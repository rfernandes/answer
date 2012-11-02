#include "anubis/webservice/webmodule/Authentication.hh"

#include <boost/program_options.hpp>
#include <wps/users/UserSession.hh>
#include <wps/users/User.hh>
#include <fstream>


using namespace std;
namespace po = boost::program_options;

namespace anubis {
namespace webservice {
namespace webmodule {

//XXX: HARD-CODED POINTS: 2

Authentication::Authentication() {

    const string configFile ( "/opt/wps/etc/wps.conf" );
    ifstream config ( configFile.c_str() );
    _timeout_secs = 5;

    po::options_description wpsGlobalConfig ( "WPS Configuration" );
    wpsGlobalConfig.add_options()
    ( "memcache.hostname", po::value<vector<string> > ( &_memcacheHosts )->required(), "Memcache hostname" )
    ( "memcache.timeout_secs",  po::value<unsigned> ( &_timeout_secs )->default_value ( _timeout_secs ), "Memcache timeout" );

    po::variables_map vm;
    po::store ( po::parse_config_file ( config, wpsGlobalConfig, true ), vm );
    po::notify ( vm );


    for ( std::vector< std::string >::const_iterator it = _memcacheHosts.begin(); it != _memcacheHosts.end(); ++it ) {

        std::size_t idx = it->rfind ( ":" );
        if ( idx != std::string::npos ) {
            try {
                std::string hostName = it->substr ( 0, idx );
                int port = boost::lexical_cast<int> ( it->substr ( idx+1 ) );
                _servers.push_back ( server_location_t ( hostName, port ) );
            } catch ( std::out_of_range ) {
                cerr << "Invalid memcache hostname:port ["<< *it <<"] "<< endl;
            }
        } else {
            _servers.push_back ( server_location_t ( *it, 11211 ) );
        }
    }

    setCacheServers();

    //TODO: Read from config
    std::ifstream fin ( "/opt/wps/etc/roles_latice" );
    if ( !fin ) {
        throw std::runtime_error ( "Could not open Roles Latice file" );
    }
    _pdp = RBAC::PDP::fromTXT ( fin );
    fin.close();
}

void Authentication::setCacheServers() {

    if ( _memcacheServer.get() == NULL ) {

        _memcacheServer = std::auto_ptr<memcache::Memcache> ( new memcache::Memcache() );
        _memcacheServer->setBehavior ( MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, _timeout_secs ); //microseconds
        _memcacheServer->setBehavior ( MEMCACHED_BEHAVIOR_SND_TIMEOUT, _timeout_secs ); //microseconds
        _memcacheServer->setBehavior ( MEMCACHED_BEHAVIOR_RCV_TIMEOUT, _timeout_secs ); //microseconds
        _memcacheServer->setBehavior ( MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, _timeout_secs ); //microseconds
        _memcacheServer->setBehavior ( MEMCACHED_BEHAVIOR_TCP_NODELAY, 1 ); //microseconds

    }

    if ( _memcacheServer.get() != NULL ) {
//         if ( _memcacheServer->getServersList().empty() ) {
            for ( std::list<server_location_t>::const_iterator it = _servers.begin(); it != _servers.end(); ++it ) {
                _memcacheServer->removeServer ( it->first, it->second );
                if ( ! _memcacheServer->addServer ( it->first, it->second ) ) {
                    cerr << " Failed to add server " << it->first << ":" << it->second << endl;
                }

            }
//         }
    }
}

answer::WebModule::FlowStatus Authentication::inFlow ( InFlowContext &context ) {



    try {
        RBAC::User user ( "" );
        //TODO: FUGLY  -  remove when refactogin audit
        wps::users::User u2 ( "-", "", "anonymous", "guest", "en-EN" );
        if ( context.getCookies().contains ( "session" ) ) {
            std::string sessionID ( context.getCookies().at ( "session" ) );

            setCacheServers();
            wps::users::UserSession uSession ( sessionID, *_memcacheServer );
            user = RBAC::User ( uSession.getUser().getUserId() );
            u2 = uSession.getUser();
            user.setRole ( _pdp->getRole ( uSession.getUser().getRoleType() ) );

        } else {
            //No session is defined, assuming unauthenticated user
            user.setRole ( _pdp->getRole ( "guest" ) );
        }
        context.getProviderStore().addProvider ( "user", user );
        context.getProviderStore().addProvider ( "username", u2.getUsername() );
        ///XXX-DEBUG: cerr << "Authentication for :" << context.getOperation().getOperationName() << endl;
    } catch ( std::runtime_error &ex ) {
        throw ModuleAuthenticationException ( ex.what() );
    }
    return OK;
}

answer::WebModule::FlowStatus Authentication::outFlow ( OutFlowContext &/*context*/ ) {
    //We currently have no outFlow handling
    return OK;
}

answer::WebModule::FlowStatus Authentication::outFlowFault ( OutFlowContext &/*context*/ ) {
    //We currently have no outFlow Fault handling
    return OK;
}

RBAC::PDP *Authentication::_pdp;

static RegisterWebModule<Authentication> _register("Authentication");

}
}
}

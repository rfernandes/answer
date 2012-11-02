#ifndef WPSMODULEAUTHENTICATION_HH
#define WPSMODULEAUTHENTICATION_HH

#include <string>
#include <anubis/webservice/WebModule.hh>
#include <anubis/webservice/Params.hh>

#include <rbac/PDP.hh>
#include <memory>
#include <libmemcached/memcached.hpp>

namespace anubis {
namespace webservice {
namespace webmodule {

class Authentication: public answer::WebModule {

    std::auto_ptr<memcache::Memcache> _memcacheServer;
    //FIXME: Experimetalist: (this should not be static)
    static RBAC::PDP *_pdp;
public:
    Authentication();
	virtual ~Authentication(){}
    virtual FlowStatus inFlow ( answer::InFlowContext &context );
    virtual FlowStatus outFlow ( answer::OutFlowContext &context );
    virtual FlowStatus outFlowFault ( answer::OutFlowContext &context );

    //FIXME: Experimetalist
    static const RBAC::PDP& getPDP() {
        return *_pdp;
    }

private:

    void setCacheServers();

    typedef std::pair<std::string, int> server_location_t;
    std::list<server_location_t> _servers;

    std::vector<std::string> _memcacheHosts;
    unsigned int _timeout_secs;
};
}
}
}

#endif // WPSMODULEAUTHENTICATION

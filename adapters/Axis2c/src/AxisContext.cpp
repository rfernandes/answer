#include <answer/Context.hh>
#include "AxisContext.hh"

namespace answer{
	namespace adapter{
		namespace axis{
			
	AxisContext::AxisContext( const axutil_env_t * axis_env,
								struct axis2_msg_ctx * msg_ctx ) :
			_cookies ( axis_env,msg_ctx ),
			_environment ( axis_env,msg_ctx ),
			_providerStore ( axis_env, msg_ctx ),
			_operation ( axis_env,msg_ctx ),
			_transport( axis_env,msg_ctx ),
			_axis_env( axis_env ),
			_msg_ctx( msg_ctx ){
				_context = this;

		}

	AxisContext::~AxisContext(){
		std::list< Cookie > cookie = _cookies.list(true);
		// Response Modifier
#ifdef AXIS2_RAW_RESPONSE
		axis2_msg_ctx_set_doing_raw ( _msg_ctx, _axis_env, _transport.redirectSet() || _transport.isBinaryTransport() );
#endif
		axutil_array_list_t* header_list = axis2_msg_ctx_get_http_output_headers ( _msg_ctx, _axis_env );
		if ( !header_list ) {
			// +1 for content-type
			header_list = axutil_array_list_create ( _axis_env, cookie.size() + 1 + _transport.redirectSet() + _transport.getHeaders().size() ); 
		}
		//AXIS2_LOG_ERROR(_axis_env->log, AXIS2_LOG_SI, "headers");

		//Cookies
		for ( std::list< Cookie >::const_iterator it = cookie.begin(); it != cookie.end(); ++it ) {
			axis2_http_header_t* new_header = axis2_http_header_create ( _axis_env, "Set-Cookie", static_cast<std::string>(*it).c_str() );
// 			AXIS2_LOG_INFO(_axis_env->log, AXIS2_LOG_SI, "header - %s:%s", "Cookie", it->toString().c_str());
			axutil_array_list_add ( header_list, _axis_env, new_header );
		}

		//Transport Headers
		for (std::multimap< std::string, std::string >::const_iterator it = _transport.getHeaders().begin(); it != _transport.getHeaders().end(); ++it){
			axis2_http_header_t* new_header = axis2_http_header_create ( _axis_env, it->first.c_str() , it->second.c_str() );
			axutil_array_list_add ( header_list, _axis_env, new_header );
		}

		//Redirect
		if ( _transport.redirectSet() ) {
			axis2_http_header_t* redirect_header = axis2_http_header_create ( _axis_env, "Location", _transport.redirect().c_str() );
			axutil_array_list_add ( header_list, _axis_env, redirect_header );
#ifdef AXIS2_HTTP_RESPONSE_MOVED_TEMPORARILY_CODE_VAL
			// if it's a location response, return code must change to 302
			axis2_msg_ctx_set_status_code ( _msg_ctx, _axis_env, AXIS2_HTTP_RESPONSE_MOVED_TEMPORARILY_CODE_VAL );
#endif
		}
		axutil_array_list_add ( header_list, _axis_env, axis2_http_header_create ( _axis_env, AXIS2_HTTP_HEADER_CONTENT_TYPE, "text/xml" ) );
		axis2_msg_ctx_set_http_output_headers ( _msg_ctx, _axis_env, header_list );
	}

//Namespace
		}
	}
}
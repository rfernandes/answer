#include "AxisTransportInfo.hh"

#include <axutil_hash.h>
#include <axis2_http_header.h>
#include <axiom_element.h>
#include <axis2_msg_ctx.h>
#include <axis2_http_transport_utils.h>
#include <axis2_handler.h>
#include <axis2_http_accept_record.h>

namespace answer{
	namespace adapter{
		namespace axis{

			AxisTransport::AxisTransport(const axutil_env_t* env, axis2_msg_ctx* msg_ctx)
			{
				//Setup transportInfo
				axutil_array_list_t * accept_record_list_aux = axis2_msg_ctx_get_http_accept_record_list ( msg_ctx, env );
				if ( accept_record_list_aux ) {
					for ( int i =0; i< axutil_array_list_size ( accept_record_list_aux, env ); ++i ) {
						axis2_http_accept_record_t* record = ( axis2_http_accept_record_t * ) axutil_array_list_get ( accept_record_list_aux, env, i );
						char * name = axis2_http_accept_record_get_name ( record, env );
						_accepts.push_back(name);
					}
				}
			}
			
			const std::string& AxisTransport::redirect(const std::string& uri){
				_uri = uri;
				return _uri;
			}
			const std::string& AxisTransport::redirect() const{
				return _uri;
			}
			
			bool AxisTransport::redirectSet() const{
				return !_uri.empty();
			}
			
			void AxisTransport::addHeader(const std::string& key, const std::string& value, bool replace){
				if (replace){
					_headers.erase(key);
				}
				_headers.insert(std::make_pair(key, value));
			}

			const std::multimap< std::string, std::string >& AxisTransport::getHeaders() const{
				return _headers;
			}

			const std::list< std::string >& AxisTransport::accepts() const
			{
				return _accepts;
			}

// 			void AxisTransport::addAccepts(const std::string& accept)
// 			{
// 				_accepts.push_back(accept);
// 			}

		}
	}
}

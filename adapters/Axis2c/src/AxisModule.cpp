#include <stdlib.h>
#include <string>
#include <stdexcept>
#include <iostream>

#include <dlfcn.h>
#include <axis2_module.h>
#include <axutil_allocator.h>
#include <axis2_conf_ctx.h>

#include <axis2_handler.h>

#include "answer/Module.hh"
#include <answer/Context.hh>
#include "AxisEnvironmentWrapper.hh"
#include "AxisCookieWrapper.hh"
#include "XmlParams.hh"

#include "AxisInFlowContext.hh"
#include "AxisOutFlowContext.hh"
#include "AxisContext.hh"

#include <glob.h>

using namespace std;
using namespace answer;

namespace answer{
namespace adapter{
namespace axis{

class ModuleLoader {
	list <void *> _soHandles;
public:
	void load ( const string& soFilepath ) {
		//opening the lib triggers the static registration on the module
		void* handle = dlopen ( soFilepath.c_str(), RTLD_NOW | RTLD_GLOBAL );

		if ( !handle ) {
			cerr << "Cannot open library ["<< soFilepath <<"]. ("<< dlerror() <<")" << endl;
			return;
		}
		_soHandles.push_back ( handle );
	}

	void unloadAll() {
		list< void* >::iterator it = _soHandles.begin();
		for ( ; it != _soHandles.end(); ++it ) {
			dlclose ( *it );
		}
	}
};
//Global module registry
ModuleLoader mloader;

#ifdef __cplusplus
extern "C"
{
#endif

	axis2_status_t AXIS2_CALL
	axutil_module_in_handler_invoke (
		struct axis2_handler * /*handler*/,
		const axutil_env_t * env,
		struct axis2_msg_ctx * msg_ctx ) {

		AxisInFlowContext flowContext ( env, msg_ctx );

		const ModuleStore::StoreT & store = ModuleStore::Instance().getStore();
		ModuleStore::StoreT::const_iterator it = store.begin();
		try {
			for ( ; it != store.end(); ++it ) {
				if ( (*it)->inFlow ( flowContext ) != Module::OK ) {
					return AXIS2_FAILURE;
				}
			}
		} catch ( ModuleAuthenticationException &ex ) {
			cerr << "Authentication IN module exception error :" << flowContext.operationInfo().service()
					<< "/" << flowContext.operationInfo().operation() << ":" << ex.what() << endl;
			axis2_msg_ctx_set_status_code ( msg_ctx, env, 401 );
			return AXIS2_FAILURE;
		} catch ( ModuleAuthorizationException &ex ) {
				cerr << "Authorization IN module exception error :" << flowContext.operationInfo().service()
						<< "/" << flowContext.operationInfo().operation() << ":" << ex.what() << endl;
			axis2_msg_ctx_set_status_code ( msg_ctx, env, 403 );
			return AXIS2_FAILURE;
		} catch ( ModuleException &ex ) {
			cerr << "General IN module exception error :" << flowContext.operationInfo().service()
					<< "/" << flowContext.operationInfo().operation() << ":" << ex.what() << endl;

			return AXIS2_FAILURE;
		}
		return AXIS2_SUCCESS;
	}
	
	AXIS2_EXTERN axis2_handler_t *AXIS2_CALL
	axutil_module_in_handler_create (
		const axutil_env_t * env,
		axutil_string_t * /*name*/ ) {
		axis2_handler_t *handler = NULL;

		AXIS2_ENV_CHECK ( env, NULL );

		handler = axis2_handler_create ( env );
		if ( !handler ) {
			return NULL;
		}

		axis2_handler_set_invoke ( handler, env, axutil_module_in_handler_invoke );

		return handler;
	}

	axis2_status_t AXIS2_CALL
	axutil_module_out_handler_invoke (
		struct axis2_handler * /*handler*/,
		const axutil_env_t * env,
		struct axis2_msg_ctx * msg_ctx ) {
		AxisOutFlowContext flowContext ( env, msg_ctx );
		const ModuleStore::StoreT & store = ModuleStore::Instance().getStore();
		ModuleStore::StoreT::const_iterator it = store.begin();
		try {
			for ( ; it != store.end(); ++it ) {
				if ( (*it)->outFlow ( flowContext ) != Module::OK ) {
					return AXIS2_FAILURE;
				}
			}
		} catch ( ModuleException &ex ) {
			cerr << "General OUT module exception error:" << flowContext.operationInfo().service()
					<< "/" << flowContext.operationInfo().operation() << ":" << ex.what() << endl;
			return AXIS2_FAILURE;
		}
		return AXIS2_SUCCESS;
	}

	AXIS2_EXTERN axis2_handler_t *AXIS2_CALL
	axutil_module_out_handler_create (
		const axutil_env_t * env,
		axutil_string_t * /*name*/ ) {
		axis2_handler_t *handler = NULL;

		AXIS2_ENV_CHECK ( env, NULL );

		handler = axis2_handler_create ( env );
		if ( !handler ) {
				return NULL;
		}

		axis2_handler_set_invoke ( handler, env, axutil_module_out_handler_invoke );

		return handler;
	}

	axis2_status_t AXIS2_CALL
	axis2_mod_module_shutdown (
		axis2_module_t * module,
		const axutil_env_t * env ) {
		mloader.unloadAll();

		if ( module->handler_create_func_map ) {
				axutil_hash_free ( module->handler_create_func_map, env );
		}

		if ( module ) {
				AXIS2_FREE ( env->allocator, module );
		}


		return AXIS2_SUCCESS;
	}

	axis2_status_t AXIS2_CALL
	axis2_mod_module_init (
		axis2_module_t * /*module*/,
		const axutil_env_t * env,
		axis2_conf_ctx_t * /*conf_ctx*/,
		axis2_module_desc_t * /*module_desc*/ ) {
		//TODO: do we need to read from a file to preserve loading order??
		const string glob_pattern ( "/opt/wps/modules/libwps_module_*.so" );
		glob_t gl;
		if ( 0 == glob ( glob_pattern.c_str(), 0, NULL, &gl ) ) {
				for ( size_t i = 0; i < gl.gl_pathc; ++i ) {
						string line ( "Loading module in " );
						line.append ( gl.gl_pathv[i] );
						AXIS2_LOG_INFO ( env->log, line.c_str() );
						mloader.load ( gl.gl_pathv[i] );
				}
				globfree ( &gl );
		}
		return AXIS2_SUCCESS;
	}

	axis2_status_t AXIS2_CALL
	axutil_module_out_fault_handler_invoke (
		struct axis2_handler * /*handler*/,
		const axutil_env_t * env,
		struct axis2_msg_ctx * msg_ctx ) {
		AXIS2_LOG_INFO ( env->log, "[Generic Module] out FAULT handler" );
		AxisOutFlowContext flowContext ( env, msg_ctx );
		const ModuleStore::StoreT & store = ModuleStore::Instance().getStore();
		ModuleStore::StoreT::const_iterator it = store.begin();
		try {
				for ( ; it != store.end(); ++it ) {
						if ( (*it)->outFlowFault ( flowContext ) != Module::OK ) {
								cerr << "General OUT FAULT module error:" << flowContext.operationInfo().service()
																	<< "/" << flowContext.operationInfo().operation() << endl;

								return AXIS2_FAILURE;
						}
				}
		} catch ( ModuleException &ex ) {
				cerr << "General OUT FAULT module exception error:" << flowContext.operationInfo().service()
													<< "/" << flowContext.operationInfo().operation() << ":" << ex.what() << endl;
				return AXIS2_FAILURE;
		}
		return AXIS2_SUCCESS;
	}

	AXIS2_EXTERN axis2_handler_t *AXIS2_CALL
	axutil_module_out_fault_handler_create (
		const axutil_env_t * env,
		axutil_string_t * /*name*/ ) {
		axis2_handler_t *handler = NULL;

		AXIS2_ENV_CHECK ( env, NULL );

		handler = axis2_handler_create ( env );
		if ( !handler ) {
				return NULL;
		}
		axis2_handler_set_invoke ( handler, env, axutil_module_out_fault_handler_invoke );
		return handler;
	}

	axis2_status_t AXIS2_CALL
	axis2_mod_module_fill_handler_create_func_map (
		axis2_module_t * module,
		const axutil_env_t * env ) {
		AXIS2_ENV_CHECK ( env, AXIS2_FAILURE );

		module->handler_create_func_map = axutil_hash_make ( env );
		if ( !module->handler_create_func_map ) {
				AXIS2_ERROR_SET ( env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE );
				return AXIS2_FAILURE;
		}

		/* add in handler */
		axutil_hash_set ( module->handler_create_func_map, "ModuleInHandler",
											AXIS2_HASH_KEY_STRING, ( const void * ) axutil_module_in_handler_create );

		/* add out handler, for successfull authentications */
		axutil_hash_set ( module->handler_create_func_map, "ModuleOutHandler",
											AXIS2_HASH_KEY_STRING, ( const void * ) axutil_module_out_handler_create );

		/* add out handler, for failed authentications */
		axutil_hash_set ( module->handler_create_func_map, "ModuleOutFaultHandler",
											AXIS2_HASH_KEY_STRING, ( const void * ) axutil_module_out_fault_handler_create );

		return AXIS2_SUCCESS;
	}

	/**
		* Module operations struct variable with functions assigned to members
		*/
	static const axis2_module_ops_t module_module_ops_var = {
		axis2_mod_module_init,
		axis2_mod_module_shutdown,
		axis2_mod_module_fill_handler_create_func_map
	};

	axis2_module_t *
	axis2_mod_module_create (
		const axutil_env_t * env ) {
		axis2_module_t *module = NULL;
		module = ( axis2_module_t* ) AXIS2_MALLOC ( env->allocator, sizeof ( axis2_module_t ) );
		/* initialize operations */
		module->ops = &module_module_ops_var;
		return module;
	}

	/**
		* Following functions are expected to be there in the module lib
		* that helps to create and remove module instances
	*/

	AXIS2_EXPORT int
	axis2_get_instance (
		axis2_module_t ** inst,
		const axutil_env_t * env ) {
		*inst = axis2_mod_module_create ( env );
		if ( ! ( *inst ) ) {
				return AXIS2_FAILURE;
		}

		return AXIS2_SUCCESS;
	}

	AXIS2_EXPORT int
	axis2_remove_instance (
		axis2_module_t * inst,
		const axutil_env_t * env ) {
		axis2_status_t status = AXIS2_FAILURE;
		if ( inst ) {
				status = axis2_mod_module_shutdown ( inst, env );
		}
		return status;
	}

#ifdef __cplusplus
}
#endif

}
}
}
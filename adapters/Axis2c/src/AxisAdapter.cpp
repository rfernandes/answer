#include "answer/OperationStore.hh"
#include "AxisEnvironmentWrapper.hh"
#include "answer/Environment.hh"

#include "AxisContext.hh"
#include "AxisTransportInfo.hh"

#include <stdio.h>
#include <iostream>
#include <set>

//////////////////////////FUNCTION TYPES

using namespace std;

#include <libgen.h>
#include <axis2_op.h>
#include <axis2_msg_ctx.h>
#include <axis2_msg_info_headers.h>
#include <axutil_property.h>
#include <axutil_properties.h>
#include <axutil_array_list.h>
#include <axutil_utils_defines.h>
#include <axis2_http_accept_record.h>
#include <axiom_data_source.h>
#include <axiom_element.h>
#include <axis2_http_header.h>
#include <axis2_http_transport.h>

/**
 * axis2_svc_skel_answer.cpp
 */

#include <axis2_svc_skeleton.h>
#include <axis2_svc.h>

/**
 * functions prototypes
 */

#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <glob.h>

using namespace answer;
using namespace std;

namespace answer{
	const char* currentService = NULL;
}

std::string getRequest ( const axutil_env_t * axis_env, axiom_node_t *content_node )
{
	string xmlString;
	if ( content_node != NULL ) {
		// Request Node preparation
		//TODO : namespace can be static
		axiom_namespace_t *		answer_ns = axiom_namespace_create ( axis_env, "http://www.w3.org/2001/12/soap-envelope","soapenv" );
		if ( answer_ns == NULL )
			throw std::runtime_error ( "Failed to create request namespace." );
		// Get the element
		axiom_element_t *content_node_element = ( axiom_element_t * ) axiom_node_get_data_element ( content_node, axis_env );
		if ( content_node_element == NULL )
			throw std::runtime_error ( "Bad request content" );

		// Namespace declaration for this node
		axiom_element_declare_namespace ( content_node_element, axis_env, content_node, answer_ns );
		axiom_element_declare_namespace_assume_param_ownership ( content_node_element, axis_env, answer_ns );

		// Request to string
		// 		axis2_char_t* request = axiom_element_to_string(content_node_element, axis_env, content_node);
		axis2_char_t* request = axiom_node_to_string ( content_node, axis_env );
		if ( request != NULL )
			xmlString.append ( request );

		axiom_namespace_free ( answer_ns, axis_env );
	}
	return xmlString;
}

#ifdef __cplusplus
extern "C"
{
#endif

	axiom_node_t* AXIS2_CALL toAxiomNode ( const axutil_env_t *env, const string& prefix, const string& operationName, axiom_node_t *content_node, axis2_msg_ctx_t *msg_ctx )
	{
// 		return NULL;
		axiom_node_t *parent = NULL;
		try {

			/* Get the string */
			string params ( getRequest ( env, content_node ) );
			
			std::size_t beg = params.find(' ');
			std::size_t end = params.find('>');
			
			params.replace(beg,  end-beg, "");
// 			cerr << "Params: " << params << endl;

			Operation& oper_ref = OperationStore::getInstance().getOperation (prefix, operationName );
			string xmlResponse = oper_ref.invoke ( params, prefix);

			// Response Node preparation
			axiom_element_t *parent_element = NULL;
			axiom_namespace_t *ns1 = NULL;
			ns1 = axiom_namespace_create ( env,"http://answer","n" );
			parent_element = axiom_element_create ( env, NULL, operationName.c_str(), ns1 , &parent );

			axiom_element_set_namespace ( parent_element, env, ns1, parent );

			axiom_node_t *current_node = NULL;
			axiom_data_source_t *data_source = axiom_data_source_create ( env, parent, &current_node );
			axutil_stream_t *stream = axiom_data_source_get_stream ( data_source, env );

			int written = axutil_stream_write ( stream, env, xmlResponse.c_str(), xmlResponse.size() );
			AXIS2_LOG_DEBUG(env->log, AXIS2_LOG_SI, "Bytes written: %d", written);
		} catch ( exception &ex ) {

			AXIS2_ERROR_SET ( env->error, AXIS2_ERROR_DATA_ELEMENT_IS_NULL, AXIS2_FAILURE );
			AXIS2_LOG_ERROR ( env->log, AXIS2_LOG_SI, ex.what() );
			return NULL;
		}
		return parent;
	}

	/* On fault, handle the fault */
	axiom_node_t* AXIS2_CALL
	axis2_svc_skel_answer_on_fault ( axis2_svc_skeleton_t */*svc_skeleton*/,
			const axutil_env_t *env, axiom_node_t *node )
	{
		axiom_node_t *error_node = NULL;
		axiom_element_t *error_ele = NULL;
		error_ele = axiom_element_create ( env, node, "fault", NULL,
										   &error_node );
		axiom_element_set_text ( error_ele, env, "answer|http://answer failed",
								 error_node );
		return error_node;
	}

	/* Free the service */
	int AXIS2_CALL
	axis2_svc_skel_answer_free ( axis2_svc_skeleton_t *svc_skeleton,
			const axutil_env_t *env )
	{
		/* Free the service skeleton */
		if ( svc_skeleton ) {
			AXIS2_FREE ( env->allocator, svc_skeleton );
			svc_skeleton = NULL;
		}
		return AXIS2_SUCCESS;
	}

	/* This method invokes the right service method */
	axiom_node_t* AXIS2_CALL
	axis2_svc_skel_answer_invoke ( axis2_svc_skeleton_t */*svc_skeleton*/,
			const axutil_env_t *env,
			axiom_node_t *content_node,
			axis2_msg_ctx_t *msg_ctx )
	{
    answer::adapter::axis::AxisContext axis( env, msg_ctx );

		/* depending on the function name invoke the corresponding  method */
		axiom_node_t *ret_node = NULL;
		axis2_op_ctx_t *operation_ctx = axis2_msg_ctx_get_op_ctx ( msg_ctx, env );
		axis2_op_t *operation = axis2_op_ctx_get_op ( operation_ctx, env );
		axutil_qname_t *op_qname = ( axutil_qname_t * ) axis2_op_get_qname ( operation, env );
		axis2_char_t *op_name = axutil_qname_get_localpart ( op_qname, env );
		axis2_char_t *op_prefix = axutil_qname_get_prefix(op_qname, env);

		try {
			ret_node = toAxiomNode ( env, op_prefix, op_name, content_node, msg_ctx );
		} catch ( exception &ex ) {
			cout << ex.what() << endl;
		}

		return ret_node;
	}

	/* Initializing the environment  */
	int AXIS2_CALL
	axis2_svc_skel_answer_init ( axis2_svc_skeleton_t */*svc_skeleton*/,
			const axutil_env_t */*env*/ )
	{
		/* Any initialization stuff of axis2_skel_answer goes here */
		return AXIS2_SUCCESS;
	}

	static const axis2_svc_skeleton_ops_t axis2_svc_skel_answer_svc_skeleton_ops_var = {
		axis2_svc_skel_answer_init,
		axis2_svc_skel_answer_invoke,
		axis2_svc_skel_answer_on_fault,
		axis2_svc_skel_answer_free,
		NULL //‘axis2_svc_skeleton_ops::init_with_conf’
	};

	/**
	 * Implementations for the functions
	 */

	void myfunc()
	{
	}

	axis2_svc_skeleton_t* AXIS2_CALL
	axis2_svc_skel_answer_create ( const axutil_env_t *env )
	{
		char *error = NULL;
		axis2_svc_skeleton_t *svc_skeleton = NULL;
		/* Allocate memory for the structs */

		Dl_info dl_info;
		dladdr ( ( void * ) myfunc, &dl_info );
		if ( ( error = dlerror() ) != NULL )  {
			cerr << "module '" << dl_info.dli_fname << "' could not be loaded: " << error << endl;
			return NULL;
		}

		// 		char
		const unsigned MAX_PATH ( 2048 );
		char soPath[MAX_PATH];
		strcpy ( soPath, dl_info.dli_fname );

		string wsSo ( dirname ( soPath ) );
		wsSo.append ( "/service.so" );

		//We set the current service, so that services are registered under this service
		answer::currentService = basename(soPath);

		dlopen ( wsSo.c_str(), RTLD_LAZY | RTLD_LOCAL);
		answer::currentService = NULL;
		if ( ( error = dlerror() ) != NULL )  {
			cerr << "module '" << wsSo << "' could not be loaded: " << error << endl;
			return NULL;
		}
		

		// 		cerr << "module loaded: " << dl_info.dli_fname << endl;

		svc_skeleton = ( axis2_svc_skeleton_t * ) AXIS2_MALLOC ( env->allocator,
					   sizeof ( axis2_svc_skeleton_t ) );

		svc_skeleton->ops = &axis2_svc_skel_answer_svc_skeleton_ops_var;
		return svc_skeleton;
	}

	/**
	 * Following block distinguish the exposed part of the dll.
	 */

	AXIS2_EXTERN int
	axis2_get_instance ( struct axis2_svc_skeleton **inst,
						 const axutil_env_t *env )
	{
		*inst = axis2_svc_skel_answer_create ( env );

		if ( ! ( *inst ) ) {
			return AXIS2_FAILURE;
		}

		return AXIS2_SUCCESS;
	}

	AXIS2_EXTERN int
	axis2_remove_instance ( axis2_svc_skeleton_t *inst,
							const axutil_env_t *env )
	{
		axis2_status_t status = AXIS2_FAILURE;
		if ( inst ) {
			status = AXIS2_SVC_SKELETON_FREE ( inst, env );
		}
		return status;
	}

#ifdef __cplusplus
}
#endif


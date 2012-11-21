#include "XmlParams.hh"
#include "answer/Exception.hh"
#include <stdexcept>
#include <iostream>

#include <axiom_soap_const.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_header.h>
#include <axiom_soap_header_block.h>
#include <axiom_soap_body.h>
#include <axis2_msg_ctx.h>

using namespace std;
using namespace answer;

string urlDecode(const string &SRC) {
	string ret;
	char ch;
	unsigned i, ii;
	for (i=0; i<SRC.length(); i++) {
		if (int(SRC[i])==37) {
			sscanf(SRC.substr(i+1,2).c_str(), "%2x", &ii);
			ch=static_cast<char>(ii);
			ret+=ch;
			i=i+2;
		} else {
			ret+=SRC[i];
		}
	}
	return (ret);
}

std::string XmlParams::getParam(const std::string& paramName, answer::MinOccurs minOcurrs) const {
	if (_rootNode == NULL)
		throw runtime_error("Request does not have any parameter");
	xmlpp::NodeSet xs = _rootNode->find(string("//") + paramName);
	if (xs.empty()) {
		if (minOcurrs == MIN_OCCURS_0)
			return "";
		throw WebMethodMissingParameter("Parameter not found in request: " +paramName);
	}
	xmlpp::Element* paramElem = (xmlpp::Element *)(*xs.begin());
	if (! paramElem->has_child_text()) {
		return "";
		//throw runtime_error("Parameter found, but was of not of correct type (XML problem ?) : " +paramName);
	}
	return urlDecode(paramElem->get_child_text()->get_content());
}

XmlParams::XmlParams(const axutil_env_t * axis_env, struct axis2_msg_ctx * msg_ctx) : _rootNode(0) {
	axiom_soap_envelope_t *soap_envelope = NULL;
	axiom_soap_body_t *soap_body = NULL;
	axiom_node_t *content_node = NULL;

	soap_envelope = axis2_msg_ctx_get_soap_envelope(msg_ctx, axis_env);
	if (soap_envelope == NULL)
	{
		std::cerr << "Failed to access SOAP envelope." << std::endl;
		return;
	}

	soap_body = axiom_soap_envelope_get_body (soap_envelope, axis_env);

	if (soap_body == NULL)
	{
		std::cerr << "Failed to access SOAP body." << std::endl;
		return;
	}

	content_node = axiom_soap_body_get_base_node(soap_body, axis_env);
	if (content_node== NULL)
	{
		std::cerr << "Failed to access SOAP body's base node." << std::endl;
		return;
	}

	string xmlString;
	
	if (content_node != NULL ) {
		// Request Node preparation
		//TODO : namespace can be static
		axiom_namespace_t *		anub_ns = axiom_namespace_create (axis_env, "http://www.w3.org/2001/12/soap-envelope","soapenv");
		if (anub_ns == NULL)
				throw std::runtime_error("Failed to create request namespace.");
		// Get the element
		axiom_element_t *content_node_element = (axiom_element_t *) axiom_node_get_data_element(content_node, axis_env);
		if (content_node_element == NULL)
				throw std::runtime_error("Bad request content");

		// Namespace declaration for this node
		axiom_element_declare_namespace(content_node_element, axis_env, content_node, anub_ns);
		axiom_element_declare_namespace_assume_param_ownership(content_node_element, axis_env, anub_ns);

		// Request to string

// 		axis2_char_t* request = axiom_element_to_string(content_node_element, axis_env, content_node);
		axis2_char_t* request = axiom_node_to_string(content_node, axis_env);
		if (request != NULL)
				xmlString.append(request);

		axiom_namespace_free(anub_ns, axis_env);
	}
	
	try {
		if (! xmlString.empty()) {
			_dom.parse_memory(xmlString);
			_rootNode = _dom.get_document()->get_root_node();
			_nodeList = _rootNode->get_children();
		}
	} catch (std::exception &ex) {
		std::cerr << "Error while parsing parameter (rethrowing): " << ex.what() << std::endl;
		throw;
	}
}

XmlParams::XmlParams(const axutil_env_t * axis_env, axiom_node_t *content_node) : _rootNode(0) {
	string xmlString;
	if (content_node != NULL ) {
		// Request Node preparation
		//TODO : namespace can be static
		axiom_namespace_t *		anub_ns = axiom_namespace_create (axis_env, "http://www.w3.org/2001/12/soap-envelope","soapenv");
		if (anub_ns == NULL)
				throw std::runtime_error("Failed to create request namespace.");
		// Get the element
		axiom_element_t *content_node_element = (axiom_element_t *) axiom_node_get_data_element(content_node, axis_env);
		if (content_node_element == NULL)
				throw std::runtime_error("Bad request content");

		// Namespace declaration for this node
		axiom_element_declare_namespace(content_node_element, axis_env, content_node, anub_ns);
		axiom_element_declare_namespace_assume_param_ownership(content_node_element, axis_env, anub_ns);

		// Request to string

		//axis2_char_t* request = axiom_element_to_string(content_node_element, axis_env, content_node);
		axis2_char_t* request = axiom_node_to_string(content_node, axis_env);
		if (request != NULL)
				xmlString.append(request);

		axiom_namespace_free(anub_ns, axis_env);
	}
	
	try {
		if (! xmlString.empty()) {
			_dom.parse_memory(xmlString);
			_rootNode = _dom.get_document()->get_root_node();
			_nodeList = _rootNode->get_children();
		}
	} catch (std::exception &ex) {
		std::cerr << "Error while parsing parameter (rethrowing): " << ex.what() << std::endl;
		throw;
	}
}

XmlParams::~XmlParams(){}

list<string> XmlParams::getParamList(const std::string& paramName, answer::MinOccurs minOcurrs) const {
	if (_rootNode == NULL)
		throw runtime_error("Request does not have any parameter");

	list<string> ret;

	xmlpp::NodeSet xs = _rootNode->find(string("//") + paramName);
	if (xs.empty()) {
		if (minOcurrs == MIN_OCCURS_0)
			return ret;
		throw WebMethodMissingParameter("Parameter not found in request: " +paramName);
	}

	for (vector< xmlpp::Node* >::const_iterator it = xs.begin(); it != xs.end(); ++it) {
		string aux(((xmlpp::Element *)(*it))->get_child_text()->get_content());
		ret.push_back(aux);
	}
	return ret;
}

list< map <string,string> > XmlParams::getParamMapList(const std::string& paramName, answer::MinOccurs minOcurrs) const {
	if (_rootNode == NULL)
		throw runtime_error("Request does not have any parameter");

	list< map <string,string> >  ret;

	xmlpp::NodeSet xs = _rootNode->find(string("//") + paramName);
	if (xs.empty()) {
		if (minOcurrs == MIN_OCCURS_0)
			return ret;
		throw WebMethodMissingParameter("Parameter not found in request: " +paramName);
	}

	for (vector< xmlpp::Node* >::const_iterator it = xs.begin(); it != xs.end(); ++it) {
		const xmlpp::Node* node = *it ;
		if (node == NULL)
			continue;

		const xmlpp::Node::NodeList nl = node->get_children();
		map <string, string> m;

		for (list< xmlpp::Node* >::const_iterator nt = nl.begin(); nt != nl.end(); ++nt) {
			const xmlpp::Element* element = dynamic_cast<const xmlpp::Element*>(*nt);
			if (element == NULL)
					continue;
			if ( ! element->has_child_text()) {
					m.insert(std::pair<std::string, std::string>(element->get_name(), ""));
					continue;
			}
			m.insert(std::pair<std::string, std::string>(element->get_name(), element->get_child_text()->get_content()));
		}
		ret.push_back(m);
	}
	return ret;
}


map <string,string> XmlParams::getParamMap(const std::string& paramName, answer::MinOccurs /*minOcurrs*/) const {
	
	//TODO: to use minOcurrs
	if (_rootNode == NULL)
		throw runtime_error("Request does not have any parameter");

	map <string,string>   ret;

	list< map <string,string> >  l = getParamMapList(paramName);

	if (l.size() != 1) {
			throw WebMethodMissingParameter("Complex parameter not found in request: " +paramName);
	}
	ret = l.front();
	return ret;
}

set<string> XmlParams::getParamSet(const std::string& paramName, answer::MinOccurs minOcurrs) const 
{
	if (_rootNode == NULL)
		throw runtime_error("Request does not have any parameter");

	std::set<string> ret;

	xmlpp::NodeSet xs = _rootNode->find(string("//") + paramName);
	if (xs.empty()) {
		if (minOcurrs == MIN_OCCURS_0)
			return ret;
		throw WebMethodMissingParameter("Parameter not found in request: " +paramName);
	}

	for (vector< xmlpp::Node* >::const_iterator it = xs.begin(); it != xs.end(); ++it) {
		string aux(((xmlpp::Element *)(*it))->get_child_text()->get_content());
		ret.insert(aux);
	}
	return ret;
}
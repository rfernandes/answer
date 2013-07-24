<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" xmlns:str="http://exslt.org/strings">
<xsl:output method="xml" version="1.0" indent="yes" />

<xsl:param name="project_name" select="project_name"/>

<!-- variable and template to identify type of request -->
<xsl:variable name="collectionTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/collectionTypes"/>
<xsl:variable name="wsdlTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/wsdlMappings"/>

<xsl:template name="dataElementIsComplex">
<xsl:param name="parameter" select="1"/>
<xsl:if test="$parameter != ''">
<xsl:for-each select="document( concat( $parameter, '.xml'))//sectiondef[@kind='public-attrib']/memberdef[@kind='variable']">
<xsl:variable name="type" select="type" />
<!-- Define a var that contains something like std::list or vector -->
<xsl:variable name="collectionType" select="substring-before(type/text(),'&lt;')" />
<!-- Use context instead of var -->
	<xsl:choose>
		<xsl:when test="$collectionTypes/item[@value=$collectionType]">
			<xsl:value-of select="true()"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:choose>
				<xsl:when test="$wsdlTypes/pair[@key=$type]">
					<xsl:call-template name="dataElementIsComplex">
						<xsl:with-param name="parameter" select="type/ref[1]/@refid"/>
					</xsl:call-template>
				</xsl:when>
			</xsl:choose>
		</xsl:otherwise>
	</xsl:choose>
	</xsl:for-each>
</xsl:if>
</xsl:template>

<xsl:template match="/">
<service name="{$project_name}">
	<parameter name="ServiceClass"><xsl:value-of select="$project_name"/></parameter>
	<description><xsl:value-of select="$project_name"/> Service</description>
<xsl:for-each select="//memberdef[@kind='function' and (contains(type, 'RegisterWebMethod') or contains(name, 'ANSWER_REGISTER_OPERATION'))]">

<xsl:variable name="operation_name">
	<xsl:choose>
		<xsl:when test="contains(type, 'RegisterWebMethod')"><!-- Registration using class -->
			<xsl:value-of select="translate(param[1]/type, '&quot;', '')"/>
		</xsl:when>
		<xsl:when test="starts-with(name, 'ANSWER_REGISTER_OPERATION')"><!-- Registration using the helper macro -->
			<xsl:value-of select="str:tokenize(param[1]/type,'::')[last()]"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:message terminate="yes">Strange registration mechanism found!</xsl:message>
		</xsl:otherwise>
	</xsl:choose>
</xsl:variable>

<xsl:variable name="service_request_class_ref">
	<xsl:choose>
		<xsl:when test="contains(type, 'RegisterWebMethod')"><!-- Registration using class -->
			<xsl:value-of select="document( concat( type/ref[1]/@refid, '.xml' ))//memberdef[name='operator()']/param/type/ref[1]/@refid"/>
		</xsl:when>
		<xsl:when test="starts-with(name, 'ANSWER_REGISTER_OPERATION')"><!-- Registration using the helper macro -->
			<!-- Remove the ending 32char hash -->
			<xsl:value-of select="document( concat( substring(param[1]/type/ref/@refid, 0, string-length(param[1]/type/ref/@refid) - 32 - 2), '.xml' ))//memberdef[name=$operation_name]/param/type/ref[1]/@refid"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:message terminate="yes">Strange registration mechanism found!</xsl:message>
		</xsl:otherwise>
	</xsl:choose>
</xsl:variable>

	<operation name="{$operation_name}" uri_ns="http://{$project_name}" uri_prefix="{$project_name}">
		<parameter name="wsamapping">urn:<xsl:value-of select="$operation_name"/></parameter>

		<xsl:variable name="test">
			<xsl:call-template name="dataElementIsComplex">
				<xsl:with-param name="parameter" select="$service_request_class_ref"/>
			</xsl:call-template>
		</xsl:variable>

		<xsl:choose>
			<xsl:when test="boolean(string($test)) or contains(param[2]/type,'POST') or contains(param[3]/type,'POST') or contains(name, '_POST_')">
				<parameter name="RESTMethod">POST</parameter>
				<parameter name="RESTLocation"><xsl:value-of select="$operation_name"/>/</parameter>
			</xsl:when>
			<xsl:otherwise>
				<parameter name="RESTMethod">GET</parameter>
				<parameter name="RESTLocation"><xsl:value-of select="$operation_name"/><xsl:if test="$service_request_class_ref != ''">
				<xsl:for-each select="document( concat( $service_request_class_ref, '.xml' ))//memberdef[@kind='variable']">
						<xsl:text>/{</xsl:text><xsl:value-of select="name"/><xsl:text>}</xsl:text>
				</xsl:for-each></xsl:if>
				<xsl:text>/</xsl:text>
				</parameter>
			</xsl:otherwise>
		</xsl:choose>
	</operation>

<!-- Request -->
<!--
<xsl:if test="$service_request_class_ref != ''">
<xsl:value-of select="$operation_name" />_json = {

};
</xsl:if>-->

</xsl:for-each>
</service>

</xsl:template>
</xsl:stylesheet>

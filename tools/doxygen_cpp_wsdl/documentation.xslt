<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"  xmlns:ex="http://exslt.org/common" xmlns:str="http://exslt.org/strings" exclude-result-prefixes="ex str">
<xsl:output method="xml" version="1.0" indent="yes" omit-xml-declaration="yes" />

<xsl:param name="project_name" select="project_name"/>

<xsl:variable name="strictDebug">yes</xsl:variable>

<!-- String replace function, which isn't builtin until xslt 2.0 (from http://stackoverflow.com/questions/7520762/xslt-1-0-string-replace-function) -->
<xsl:template name="replace-string">
	<xsl:param name="text"/>
	<xsl:param name="replace"/>
	<xsl:param name="with"/>
	<xsl:choose>
		<xsl:when test="contains($text,$replace)">
			<xsl:value-of select="substring-before($text,$replace)"/>
			<xsl:value-of select="$with"/>
			<xsl:call-template name="replace-string">
				<xsl:with-param name="text" select="substring-after($text,$replace)"/>
				<xsl:with-param name="replace" select="$replace"/>
				<xsl:with-param name="with" select="$with"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$text"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Auxiliary variables to identify type of request (GET vs POST) -->
<xsl:variable name="collectionTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/collectionTypes"/>
<xsl:variable name="wsdlTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/wsdlMappings"/>

<!-- Template to identify whether a class's variables are basic data types	-->
<!-- Arguments																-->
<!-- 	fileID - id reference of a class or struct file						-->
<!-- Return true if type is complex, nothing otherwise						-->
<xsl:template name="dataElementIsComplex">
<xsl:param name="fileID"/>
<xsl:if test="$fileID != ''">
<xsl:for-each select="document( concat( $fileID, '.xml'))//sectiondef[@kind='public-attrib']/memberdef[@kind='variable']">
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

<!-- Template to write an HTML tag escaped		-->
<!-- Arguments									-->
<!-- 	tag - name of the html tag to be formed	-->
<!-- 	node - content of the tag				-->
<!-- Result										-->
<!-- 	<tag>node</tag>							-->
<xsl:template name="htmlEscape">
	<xsl:param name="tag" />
	<xsl:param name="node" />
	&lt;<xsl:value-of select="$tag" />&gt;
	<xsl:copy-of select="$node" />
	&lt;/<xsl:value-of select="$tag" />&gt;
</xsl:template>

<!-- Template to write an HTML comment escaped	-->
<!-- Arguments									-->
<!-- 	comment - string content of the comment	-->
<!-- Result										-->
<!--	&lt;&#33;&#45;&#45;comment&#45;&#45;&gt;-->
<xsl:template name="htmlComment">
	<xsl:param name="comment" />
	&lt;!-- <xsl:value-of select="$comment" /> --&gt;
</xsl:template>

<!-- Template to identify data types and their composition -->
<!-- It takes no arguments, instead it works on 
					nodes with the type tag when requested -->
<xsl:template name="disambiguateType" match="type">
			<xsl:variable name="type" select="text()"/>
			<xsl:variable name="collectionType" select="translate(substring-before($type, '&lt;'),' ', '')" />
			<xsl:variable name="collectionTemplateType" >
				<xsl:value-of select="normalize-space(substring-before(substring-after($type, '&lt;'), '&gt;'))"/>
				<xsl:if test="substring($type,string-length($type)-3)=' > >'">
					&gt;
				</xsl:if>
			</xsl:variable>
			
<!--			<xsl:message>Disambiguate <xsl:value-of select="$type" /> - <xsl:value-of select="$collectionType" /><xsl:value-of select="$collectionTemplateType" /></xsl:message>-->
			
			<xsl:choose> <!-- This xsl:choose differentiates collection data types (lists, maps, etc.) -->
				<xsl:when test="$collectionTypes/item[@value=$collectionType]">
					<xsl:choose> <!-- Since maps are a special case of collections, they must be dealt with separately -->
						<xsl:when test="$collectionType='map' or $collectionType ='std::map'">
							<xsl:variable name="mapFirst">
								<memberdef>
									<name>first</name>
									<type><xsl:value-of select="normalize-space(substring-before($collectionTemplateType,','))" /></type>
								</memberdef>
							</xsl:variable>
							<xsl:variable name="mapSecond">
								<memberdef>
									<name>second</name>
									<type><xsl:value-of select="normalize-space(substring-after($collectionTemplateType,','))" /></type>
								</memberdef>
							</xsl:variable>
							<dl>
							<dd>
								<xsl:call-template name="htmlComment">
									<xsl:with-param name="comment" >
										One or more pairs of first and second elements
									</xsl:with-param>
								</xsl:call-template>
							</dd>
							<xsl:apply-templates select="ex:node-set($mapFirst)/memberdef" />
							<xsl:apply-templates select="ex:node-set($mapSecond)/memberdef" />
							</dl>
						</xsl:when>
						<xsl:otherwise>
							<xsl:variable name="inner">
								<type> <xsl:choose>
									<xsl:when test="ref[1]/@refid">
										<ref refid="{ref[1]/@refid}" kindref="{ref[1]/@kindref}">
											<xsl:value-of select="$collectionTemplateType" />
										</ref>
									</xsl:when>
									<xsl:otherwise>
										<xsl:value-of select="$collectionTemplateType" />
									</xsl:otherwise>
									</xsl:choose> </type>
							</xsl:variable>
							<xsl:apply-templates select="ex:node-set($inner)/type" />
						</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
				<xsl:when test="$wsdlTypes/pair[@key=$type]"> <!-- Whether this element is of a basic data type -->
					<xsl:value-of select="substring-after($wsdlTypes/pair[@key=$type]/@value,':')"/>
				</xsl:when>
				<xsl:when test="ref[1]/@refid"> <!-- Whether this element is of a complex data type -->
					<xsl:apply-templates select="ref[1]" />
				</xsl:when>
				<xsl:otherwise>
					string <!-- For now, complex types defined outside the project should be considered strings -->
				</xsl:otherwise>
			</xsl:choose>
</xsl:template>

<!-- Template to write data types elements in HTML				-->
<!-- It takes no arguments, instead it works on 
					nodes with the memberdef tag when requested	-->
<xsl:template name="serializeElement" match="memberdef">
	<!-- Define a var that contains something like std::list of vector -->
	<xsl:variable name="collectionType" select="translate(substring-before(type/text(), '&lt;'),' ', '')" />
	<xsl:variable name="collectionTemplateType" select="normalize-space(substring-after(substring-before(type/text(), '&gt;'), '&lt;'))" />
<!--	<xsl:message>memberdef <xsl:value-of select="name" /> .</xsl:message>-->
			
	<!-- Identify repeatable elements (ie those who are mapped to collection types) through a comment-->
	<xsl:if test="$collectionTypes/item[@value=$collectionType] and $collectionType!='map' and $collectionType!='std::map'">
		<dd>
		<xsl:call-template name="htmlComment">
			<xsl:with-param name="comment" >
				One or more repetitions of <xsl:value-of select="name"/> elements
			</xsl:with-param>
		</xsl:call-template>
		</dd>
	</xsl:if>
	<!-- the payload xsl:variable keeps the actual processed data type for the variable -->
	<dd>
	<xsl:call-template name="htmlEscape"> <!-- Create the actual html output for this element -->
		<xsl:with-param name="tag" select="name"/>
		<xsl:with-param name="node">
			<xsl:apply-templates select="type"/>
		</xsl:with-param>
	</xsl:call-template>
	</dd>
</xsl:template>

<!-- Template to write data type from external references in XML	-->
<!-- Arguments														-->
<!-- 	fileID - id reference of a class or struct file				-->
<!-- 	memberID - id reference of a specific type (Optional)		-->
<!-- 	parentType - class name of the provided fileId				-->
<!-- Result															-->
<!-- 	<varname>vartype</varname>									-->
<!-- 	<complexvarname>											-->
<!-- 		<varname>vartpe</vartype>								-->
<!-- 	</complexvarname>											-->
<xsl:template match="ref">
	<xsl:variable name="fileID">
		<xsl:choose>
			<xsl:when test="@kindref='compound'">
				<xsl:value-of select="@refid" />
			</xsl:when>
			<xsl:when test="@kindref='member'">
				<xsl:value-of select="substring-before( @refid, concat('_', str:tokenize( @refid , '_')[last()]) )" />
			</xsl:when>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="memberID">
		<xsl:if test="@kindref='member'">
			<xsl:value-of select="@refid" />
		</xsl:if>
	</xsl:variable>
	<xsl:variable name="parentType" select="text()" />

	<xsl:if test="$fileID != ''">
<!--	<xsl:message><xsl:value-of select="$fileID" />, <xsl:value-of select="$memberID" />, <xsl:value-of select="$parentType" /></xsl:message>-->
	<xsl:choose><!-- The next xsl:when happens when a class has no public variables and must be identified by the type from which it is constructed -->
	<xsl:when test="( document( concat( $fileID, '.xml'))//sectiondef[@kind='public-func']/memberdef[@kind='function'][name=$parentType] and not(document( concat( $fileID, '.xml'))//sectiondef[@kind='public-attrib']/memberdef[@kind='variable']) ) or document( concat( $fileID, '.xml'))//sectiondef[@kind='public-func']/memberdef[@kind='typedef'][name=$parentType]">
		<xsl:variable name="targetNode" select="document( concat( $fileID, '.xml'))//sectiondef[@kind='public-func']/memberdef[@kind='function'][name=$parentType] | document( concat( $fileID, '.xml'))//sectiondef[@kind='public-func']/memberdef[@kind='typedef'][name=$parentType]" />
		<!-- argtype is the argument of the constructor parsed to its basic form (without const and &) -->
		<xsl:variable name="argtype">
			<xsl:variable name="auxstr">
				<xsl:call-template name="replace-string">
					<xsl:with-param name="text" select="ex:node-set($targetNode)/param[1]/type" />
					<xsl:with-param name="replace" select="' &amp;'" />
					<xsl:with-param name="with" select="''"/>
				</xsl:call-template>
			</xsl:variable>
			<xsl:call-template name="replace-string">
				<xsl:with-param name="text" select="$auxstr" />
				<xsl:with-param name="replace" select="'const '" />
				<xsl:with-param name="with" select="''"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:choose>
		<!-- The payload should only be identified this way when the constructor takes a single basic data type argument -->
		<xsl:when test="$wsdlTypes/pair[@key=$argtype]">
			<xsl:value-of select="substring-after($wsdlTypes/pair[@key=$argtype]/@value,':')"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:message terminate="{$strictDebug}">[Doc] Couldn't identify payload for <xsl:value-of select="$parentType" /></xsl:message>
		</xsl:otherwise>
		</xsl:choose>
	</xsl:when>
	<xsl:when test="@kindref='member'">
<!--	<xsl:message>MEMBER</xsl:message>-->
		<dl> <!-- The output is comprised of definition lists for automatic indentation of inner types -->
		<xsl:for-each select="document( concat( $fileID, '.xml'))//memberdef[@id=$memberID]">
			<xsl:choose>
				<xsl:when test="@kind='typedef'">
					<xsl:apply-templates select="type" />
				</xsl:when>
				<xsl:otherwise>					
					<xsl:apply-templates select="current()" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>	
		</dl>
	</xsl:when>
	<!-- The next xsl:when happens when a class has public variables from which its payload elements can be identified -->
	<xsl:when test="@kindref='compound'">
<!--	<xsl:message>COMPOUND</xsl:message>-->
		<dl> <!-- The output is comprised of definition lists for automatic indentation of inner types -->
		<xsl:for-each select="document( concat( $fileID, '.xml'))//sectiondef[@kind='public-attrib']/memberdef[@kind='variable']">
			<xsl:apply-templates select="current()" />
		</xsl:for-each>	
		</dl>
	</xsl:when>
	<xsl:otherwise>
		<xsl:message terminate="{$strictDebug}">[Doc] Couldn't identify payload for <xsl:value-of select="$parentType" /></xsl:message>
	</xsl:otherwise>
	</xsl:choose>
	</xsl:if>
</xsl:template>

<!-- Template to write descriptions from a method node 					-->
<!-- Arguments															-->
<!-- 	node - The method definition node								-->
<!-- Result																-->
<!-- 	The html output of descriptions as paragraphs preceded 
		by a Description header or nothing if there are no descriptions	-->
<xsl:template name="desc">
		<xsl:param name="node" />
		<xsl:if test="boolean(normalize-space($node/briefdescription/para)) or boolean(normalize-space($node/detaileddescription/para))">
			<p>
				<h4>Description</h4>
				<xsl:if test="boolean(normalize-space($node/briefdescription/para))">
					<p><xsl:value-of select="$node/briefdescription" /></p>
				</xsl:if>
				<xsl:if test="boolean(normalize-space($node/detaileddescription/para))">
					<p><xsl:value-of select="$node/detaileddescription" /></p>
				</xsl:if>
			</p>
		</xsl:if>
</xsl:template>

<!-- Main template to process the webservices -->
<xsl:template match="/">
<html>
	<head>
	<title><xsl:value-of select="$project_name"/></title>
	</head>
	
	<body>
		
<xsl:for-each select="//memberdef[@kind='function' and (contains(type, 'RegisterWebMethod') or contains(name, 'ANSWER_REGISTER_OPERATION'))]">

<xsl:variable name="operation_name">
	<xsl:choose>
		<xsl:when test="contains(type, 'RegisterWebMethod')"><!-- Registration using class -->
			<xsl:value-of select="translate(param[1]/type, '&quot;', '')"/>
		</xsl:when>
		<xsl:when test="starts-with(name, 'ANSWER_REGISTER_OPERATION')"><!-- Registration using the helper macro -->
			<xsl:value-of select="substring-after(param[1]/type,'::')"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:message terminate="{$strictDebug}">Strange registration mechanism found!</xsl:message>
		</xsl:otherwise>
	</xsl:choose>
</xsl:variable>

<xsl:variable name="service_class">
	<xsl:choose>
		<xsl:when test="contains(type, 'RegisterWebMethod')"><!-- Registration using class -->
			<xsl:variable name="filename">
				<xsl:choose>
					<xsl:when test="type/ref[1]/@kindref='compound'" >
						<xsl:value-of select="type/ref[1]/@refid"/>
					</xsl:when>
					<xsl:when test="type/ref[1]/@kindref='member'" ><xsl:message><xsl:value-of select="type/ref/@refid" /> >>> <xsl:value-of select="substring-before( type/ref/@refid, concat('_', str:tokenize( param[1]/type/ref/@refid , '_')[last()]) )" /></xsl:message>
	
						<xsl:value-of select="substring-before( type/ref/@refid, concat('_', str:tokenize( type/ref/@refid , '_')[last()]) )"/>
					</xsl:when>
				</xsl:choose>
			</xsl:variable>
			<xsl:copy-of select="document( concat( $filename , '.xml' ) )//memberdef[name='operator()']"/>
		</xsl:when>
		<xsl:when test="starts-with(name, 'ANSWER_REGISTER_OPERATION')"><!-- Registration using the helper macro -->
			<xsl:variable name="filename">
				<xsl:choose>
					<xsl:when test="param[1]/type/ref[1]/@kindref='compound'" >
						<xsl:value-of select="concat( param[1]/type/ref[1]/@refid, '.xml' )"/>
					</xsl:when>
					<xsl:when test="param[1]/type/ref[1]/@kindref='member'" >
						<xsl:value-of select="substring-before( param[1]/type/ref/@refid, concat('_', str:tokenize( param[1]/type/ref/@refid , '_')[last()]) )"/>
					</xsl:when>
				</xsl:choose>
			</xsl:variable>
			<xsl:copy-of select="document( concat( $filename , '.xml' ))//memberdef[name=$operation_name]"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:message terminate="{$strictDebug}">Strange registration mechanism found!</xsl:message>
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

<xsl:variable name="test">
	<xsl:call-template name="dataElementIsComplex">
		<xsl:with-param name="fileID" select="$service_request_class_ref"/>
	</xsl:call-template>
</xsl:variable>

<xsl:variable name="descriptions">
	<xsl:call-template name="desc">
		<xsl:with-param name="node" select="ex:node-set($service_class)/memberdef"/>
	</xsl:call-template>
</xsl:variable>

		<h2>
			<xsl:value-of select="$operation_name"/>
		</h2>
		<hr align="left" width="{string-length($operation_name)*20}pixels"/>
		<xsl:copy-of disable-output-escaping="yes" select="$descriptions"/>
		<p>
			<h4>Request Type</h4>
			<xsl:choose>
				<!-- contains(param[2]/type,'POST') the parameter will chage to make way for multiple functions per class -->
				<xsl:when test="boolean(string($test)) or contains(param[2]/type,'POST') or contains(param[3]/type,'POST') or contains(name, '_POST_')">
					POST
				</xsl:when>
				<xsl:otherwise>
					GET
				</xsl:otherwise>
			</xsl:choose>
		</p>
		
		<xsl:if test="ex:node-set($service_class)/memberdef/param[1]/type/ref[1]">
			<p>
				<h4>Request Payload</h4>
				<p>
					<xsl:apply-templates select="ex:node-set($service_class)/memberdef/param[1]/type/ref[1]" />
				</p>
			</p>
		</xsl:if>

		<xsl:if test="ex:node-set($service_class)/memberdef/type/ref[1]">
			<p>
				<h4>Response Payload</h4>
				<p>
					<xsl:apply-templates select="ex:node-set($service_class)/memberdef/type/ref[1]" />
				</p>
			</p>
		</xsl:if>
	<br/>
	<hr/>
	<br/>
	
</xsl:for-each>

	</body>
</html>
</xsl:template>
</xsl:stylesheet>

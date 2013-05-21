<!-- XSLT script to combine the generated output into a single file.
     If you have xsltproc you could use:
     xsltproc combine.xslt index.xml >all.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" xmlns:ex="http://exslt.org/common"
xmlns:str="http://exslt.org/strings" exclude-result-prefixes="ex str">
<xsl:output method="text" version="1.0" indent="no" />
<xsl:variable name="collectionTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/collectionTypes"/>
<xsl:variable name="wsdlTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/wsdlMappings"/>
<xsl:variable name="wrapperTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/wrapperTypes"/>

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

<!-- Template to identify data types and their composition -->
<!-- It takes no arguments, instead it works on
					nodes with the type tag when requested -->
<xsl:template name="disambiguateType" match="type">
	<xsl:variable name="type" select="."/>
	<xsl:variable name="templateType" select="translate(substring-before($type, '&lt;'),' ', '')" />
	<xsl:variable name="templateParameterType" >
		<xsl:value-of select="normalize-space(substring-before(substring-after($type, '&lt;'), '&gt;'))"/>
		<xsl:if test="substring($type,string-length($type)-2)='> >'"> &gt;</xsl:if>
	</xsl:variable>

<!-- 	<xsl:message>Type [<xsl:value-of select="$type" />]</xsl:message> -->
<!-- 	<xsl:message>TemplateType [<xsl:value-of select="$templateType" />]</xsl:message> -->
<!-- 	<xsl:message>TemplateParameterType [<xsl:value-of select="$templateParameterType" />]</xsl:message> -->

	<xsl:variable name="setOptional" >
		<xsl:if test="$templateType='boost::optional' or $templateType='optional'">, "minOccurs" : 0</xsl:if>
	</xsl:variable>

	<xsl:choose> <!-- This xsl:choose differentiates collection data types (lists, maps, etc.) -->
		<xsl:when test="$wrapperTypes/item[@value=$templateType]">
			<xsl:variable name="inner">
				<type><xsl:choose>
					<xsl:when test="ref[1]/@refid">
						<ref refid="{ref[1]/@refid}" kindref="{ref[1]/@kindref}">
							<xsl:value-of select="$templateParameterType" />
						</ref>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$templateParameterType" />
					</xsl:otherwise>
					</xsl:choose></type>
			</xsl:variable>
			<xsl:apply-templates select="ex:node-set($inner)/type" />
		</xsl:when>
		<xsl:when test="$collectionTypes/item[@value=$templateType]">
			[<xsl:choose> <!-- Since maps are a special case of collections, they must be dealt with separately -->
				<xsl:when test="$templateType='map' or $templateType ='std::map'">
					<xsl:variable name="mapFirst">
						<memberdef>
							<name>first</name>
							<type><xsl:value-of select="normalize-space(substring-before($templateParameterType,','))" /></type>
						</memberdef>
					</xsl:variable>
					<xsl:variable name="mapSecond">
						<memberdef>
							<name>second</name>
							<type><xsl:value-of select="normalize-space(substring-after($templateParameterType,','))" /></type>
						</memberdef>
					</xsl:variable>
					{
					<xsl:apply-templates select="ex:node-set($mapFirst)/memberdef" />,
					<xsl:apply-templates select="ex:node-set($mapSecond)/memberdef" />
					}
				</xsl:when>
				<xsl:otherwise>
					<xsl:variable name="inner">
						<type><xsl:choose>
							<xsl:when test="ref[1]/@refid">
								<ref refid="{ref[1]/@refid}" kindref="{ref[1]/@kindref}">
									<xsl:value-of select="$templateParameterType" />
								</ref>
							</xsl:when>
							<xsl:otherwise>
								<xsl:value-of select="$templateParameterType" />
							</xsl:otherwise>
							</xsl:choose></type>
					</xsl:variable>
					<xsl:apply-templates select="ex:node-set($inner)/type" />
				</xsl:otherwise>
			</xsl:choose>]
		</xsl:when>
		<xsl:when test="$wsdlTypes/pair[@key=$type]"> <!-- Whether this element is of a basic data type -->
			"<xsl:value-of select="substring-after($wsdlTypes/pair[@key=$type]/@value,':')"/>"
		</xsl:when>
		<xsl:when test="ref[1]/@refid"> <!-- Whether this element is of a complex data type -->
			<xsl:apply-templates select="ref[1]" />
		</xsl:when>
		<xsl:otherwise>
			<!-- "string" Complex types were being defined outside the project as strings -->
			"complex"
		</xsl:otherwise>
	</xsl:choose>
<!-- Flag optional (or var is empty) -->
	<xsl:value-of select="$setOptional"/>
</xsl:template>

<!-- Template to write data types elements in JSON				-->
<!-- It takes no arguments, instead it works on
					nodes with the memberdef tag when requested	-->
<xsl:template name="serializeElement" match="memberdef">
<!-- 	<xsl:message>-=-=-=-=-=-=-</xsl:message> -->
<!-- 	<xsl:message>Name [<xsl:value-of select="name" />]</xsl:message> -->
	"<xsl:value-of select="name"/>" : {"type" :<xsl:apply-templates select="type"/>}
</xsl:template>

<!-- Template to write data type from external references in XML	-->
<!-- Arguments are infered from node type ("ref" in this case		-->
<!-- 	fileID - id reference of a class or struct file				-->
<!-- 	memberID - id reference of a specific type (Optional)		-->
<!-- 	parentType - class name of the provided fileId				-->
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
	<xsl:choose>
	<!-- The next xsl:when happens when a class has no public variables and must be identified by the type from which it is constructed -->
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
			"<xsl:value-of select="substring-after($wsdlTypes/pair[@key=$argtype]/@value,':')"/>"
		</xsl:when>
		<xsl:otherwise>
			<xsl:message terminate="no">Couldn't identify payload for <xsl:value-of select="$parentType" /></xsl:message>
		</xsl:otherwise>
		</xsl:choose>
	</xsl:when>
	<!-- These next xsl:whens happens when a class/struct has public variables from which its payload elements can be identified -->
	<xsl:when test="@kindref='member'">
		<xsl:variable name="memberNode" select="document( concat( $fileID, '.xml'))//memberdef[@id=$memberID]" />
		<xsl:choose>
			<xsl:when test="ex:node-set($memberNode)/@kind='typedef'">
				<xsl:apply-templates select="ex:node-set($memberNode)/type" />
			</xsl:when>
			<xsl:otherwise>
				{<xsl:apply-templates select="current()" />}
			</xsl:otherwise>
		</xsl:choose>
	</xsl:when>
	<xsl:when test="@kindref='compound'">
		{<xsl:for-each select="document( concat( $fileID, '.xml'))//sectiondef[@kind='public-attrib']/memberdef[@kind='variable']">
			<xsl:apply-templates select="current()" />
			<xsl:if test="position() != last()" >, </xsl:if>
		</xsl:for-each>}
	</xsl:when>
	<xsl:otherwise>
		<xsl:message terminate="no">Couldn't identify payload for <xsl:value-of select="$parentType" /></xsl:message>
	</xsl:otherwise>
	</xsl:choose>
	</xsl:if>
</xsl:template>

<xsl:template match="/">
<xsl:for-each select="//memberdef[@kind='function' and (contains(type, 'RegisterWebMethod') or starts-with(name, 'ANSWER_REGISTER_OPERATION'))]">
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
			<xsl:message terminate="yes">Strange registration mechanism found!</xsl:message>
		</xsl:otherwise>
	</xsl:choose>
</xsl:variable>


<!-- Re quest -->
<xsl:if test="ex:node-set($service_class)/memberdef/param[1]/type/ref[1]">
<xsl:variable name="text"><xsl:value-of select="$operation_name" />_json = <xsl:apply-templates select="ex:node-set($service_class)/memberdef/param[1]/type/ref[1]" />;</xsl:variable>
<xsl:value-of select="translate($text,'&#9;&#10;','')" /><xsl:text>
</xsl:text>
</xsl:if>

<!-- Response -->
<xsl:if test="normalize-space(ex:node-set($service_class)/memberdef/type/ref[1])">
<xsl:variable name="text"><xsl:value-of select="$operation_name" />_response_json = <xsl:apply-templates select="ex:node-set($service_class)/memberdef/type/ref[1]" />;</xsl:variable>
<xsl:value-of select="translate($text,'&#9;&#10;','')" /><xsl:text>
</xsl:text>
</xsl:if>

	</xsl:for-each>
  </xsl:template>
</xsl:stylesheet>

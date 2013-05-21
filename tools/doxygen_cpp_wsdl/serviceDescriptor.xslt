<!-- XSLT script to combine the generated output into a single file. 
     If you have xsltproc you could use:
     xsltproc combine.xslt index.xml >all.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" xmlns:ex="http://exslt.org/common" xmlns:str="http://exslt.org/strings" exclude-result-prefixes="ex str">
<xsl:output method="xml" version="1.0" indent="yes" />
<xsl:param name="project_name" select="project_name"/>

<xsl:variable name="wrapperTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/wrapperTypes"/>
<xsl:variable name="collectionTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/collectionTypes"/>
<xsl:variable name="wsdlTypes" select="document('cpp_dataTypes.xml')/cppDataTypes/wsdlMappings"/>

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

<xsl:template match="string">
<input type="text" />
</xsl:template>

<!-- Template to identify data types and their composition -->
<!-- It takes no arguments, instead it works on 
					nodes with the type tag when requested -->
<xsl:template name="disambiguateType" match="memberdef">
	<xsl:variable name="type" select="type"/>	
	<xsl:variable name="template-class" select="substring-before($type,'&lt;')"/>
	<xsl:variable name="ws" value='&#32;&#13;&#10;&#09;'/>
	
	
	<xsl:variable name="type-templated">
		<xsl:value-of select="translate(substring-before(substring-after($type,'&lt;'),'&gt;'), '&#32;&#13;&#10;&#09;', '')" />
	</xsl:variable>
	
	
	<xsl:variable name="type-container">
		<xsl:value-of select="translate(substring-before($type,'&lt;'), '&#32;&#13;&#10;&#09;', '')" />
	</xsl:variable>
	
<!--
	
	<xsl:if test="string-length($type-templated) > 0 ">
		<debug1> (<xsl:value-of select="$type"/>) / (<xsl:value-of select="$type-templated"/>) : (<xsl:value-of select="string-length($type-templated)"/>) </debug1>
	</xsl:if>
		<xsl:if test="string-length($type-container) > 0 ">
		<debug2> (<xsl:value-of select="$type"/>) / (<xsl:value-of select="$type-container"/>) : (<xsl:value-of select="string-length($type-container)"/>) </debug2>
	</xsl:if>
-->
	
	<xsl:variable name="description">
		<xsl:choose> <!-- This xsl:choose differentiates collection data types (lists, maps, etc.) -->
			<xsl:when test="substring-before(briefdescription/para,'.') != ''">
				<xsl:value-of select="substring-before(briefdescription/para,'.')"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="name"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	
	
	
	<xsl:variable name="collectionType" select="translate(substring-before(type, '&lt;'), '&#32;&#13;&#10;&#09;', '')" />
	<xsl:variable name="collectionTemplateType" >
		<xsl:value-of select="normalize-space(substring-before(substring-after(type, '&lt;'), '&gt;'))"/>
		<xsl:if test="substring(type,string-length(type)-3)=' > >'">
			&gt;
		</xsl:if>
	</xsl:variable>
	<xsl:choose> <!-- This xsl:choose differentiates collection data types (lists, maps, etc.) -->
		<xsl:when test="$collectionTypes/item[@value=$collectionType]">
			<collection>
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
					<map>
						<first><xsl:apply-templates select="ex:node-set($mapFirst)/memberdef" /></first>
						<second><xsl:apply-templates select="ex:node-set($mapSecond)/memberdef" /></second>
					</map>
				</xsl:when>
				<xsl:otherwise>
					<xsl:variable name="inner">
					<memberdef>
						<xsl:copy-of select="name" />
						<type>
							<xsl:choose>
								<xsl:when test="ref[1]/@refid">
									<ref refid="{ref[1]/@refid}" kindref="{ref[1]/@kindref}">
										<xsl:value-of select="$collectionTemplateType" />
									</ref>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="$collectionTemplateType" />
								</xsl:otherwise>
							</xsl:choose>
						</type>
					</memberdef>
					</xsl:variable>
					<xsl:apply-templates select="ex:node-set($inner)/memberdef" />
				</xsl:otherwise>
			</xsl:choose></collection>
		</xsl:when>
	
		<xsl:when test="$wrapperTypes/pair[@key=$type-container]"> <!-- Whether this element is of a wrapper type -->
			<xsl:element name="{translate($type-templated,':','_')}">
				<xsl:attribute name="name"><xsl:value-of select="name"/></xsl:attribute>
				<xsl:attribute name="description"><xsl:value-of select="$description"/></xsl:attribute>
			</xsl:element>
		</xsl:when>
	
		<xsl:when test="$wsdlTypes/pair[@key=$type]"> <!-- Whether this element is of a basic data type -->
			<xsl:element name="{substring-after($wsdlTypes/pair[@key=$type]/@value,':')}">
			<xsl:attribute name="name"><xsl:value-of select="name"/></xsl:attribute>
				<xsl:attribute name="description"><xsl:value-of select="$description"/></xsl:attribute>
			</xsl:element>
		</xsl:when>	
		
		<xsl:when test="ref[1]/@refid"> <!-- Whether this element is of a complex data type -->
			<xsl:apply-templates select="ref[1]" />
		</xsl:when>
		
		<xsl:otherwise>
<!--			<xsl:element name="{translate(type,':','_')}">
				<xsl:attribute name="name"><xsl:value-of select="name"/></xsl:attribute>
				<xsl:attribute name="description"><xsl:value-of select="$description"/></xsl:attribute>
			</xsl:element>-->
		</xsl:otherwise>

	</xsl:choose>
</xsl:template>

<!-- Template to write data types elements in JSON				-->
<!-- It takes no arguments, instead it works on 
					nodes with the memberdef tag when requested	-->
<!-- <xsl:template name="serializeElement" match="memberdef"> -->
<!-- 	<xsl:attribute name="type"><xsl:value-of select="translate(type, ':' , '_')"/></xsl:attribute> -->
<!-- 	<xsl:value-of select="translate(type, ':' , '_')"/> -->
<!-- <xsl:value-of select="name"/> -->
<!--	<xsl:apply-templates select="type"/>
 	<xsl:element name="{name}">
	</xsl:element> 
</xsl:template>-->

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
				<xsl:apply-templates select="current()" />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:when>
	<xsl:when test="@kindref='compound'">
		<xsl:for-each select="document( concat( $fileID, '.xml'))//sectiondef[@kind='public-attrib']/memberdef[@kind='variable']">
			<xsl:apply-templates select="current()" />
<!-- 			<xsl:if test="position() != last()" ></xsl:if> -->
		</xsl:for-each>
	</xsl:when>
	<xsl:otherwise>
		<xsl:message terminate="no">Couldn't identify payload for <xsl:value-of select="$parentType" /></xsl:message>
	</xsl:otherwise>
	</xsl:choose>
	</xsl:if>
</xsl:template>

<xsl:template match="/">

<service>
	<xsl:attribute name="name"><xsl:value-of select="$project_name"/></xsl:attribute>
<xsl:for-each select="//memberdef[@kind='function' and (contains(type, 'RegisterWebMethod') or starts-with(name, 'ANSWER_REGISTER_OPERATION'))]">
<xsl:variable name="operation_name">
	<xsl:choose>
		<xsl:when test="contains(type, 'RegisterWebMethod')"><!-- Registration using class -->
			<xsl:value-of select="translate(param[1]/type, '&quot;', '')"/>
		</xsl:when>
		<xsl:when test="starts-with(name, 'ANSWER_REGISTER_OPERATION')"><!-- Registration using the helper macro -->
			<xsl:value-of select="substring-after(param[1]/type,'::')"/>
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

<!-- Request -->
<xsl:element name="operation">
	<xsl:attribute name="name"><xsl:value-of select="$operation_name" /></xsl:attribute>
<xsl:if test="ex:node-set($service_class)/memberdef/param[1]/type/ref[1]">
<xsl:element name="request">
	<xsl:apply-templates select="ex:node-set($service_class)/memberdef/param[1]/type/ref[1]" />
</xsl:element>
</xsl:if>

<!-- Response -->
<xsl:if test="ex:node-set($service_class)/memberdef/type/ref[1]">

<xsl:element name="response">
	<xsl:attribute name="name"><xsl:value-of select="$operation_name" /></xsl:attribute>
	<xsl:apply-templates select="ex:node-set($service_class)/memberdef/type/ref[1]" />
</xsl:element>
</xsl:if>
</xsl:element>

<!-- Response -->
<!--<xsl:if test="normalize-space(ex:node-set($service_class)/memberdef/type/ref[1])">
<xsl:variable name="text"><xsl:value-of select="$operation_name" />_response_json = <xsl:apply-templates select="ex:node-set($service_class)/memberdef/type/ref[1]" />;</xsl:variable>
<xsl:value-of select="translate($text,'&#9;&#10;','')" /><xsl:text>
</xsl:text>
</xsl:if>-->

	</xsl:for-each>

</service>
  </xsl:template>
</xsl:stylesheet>

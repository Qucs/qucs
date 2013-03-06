<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- 
       This xsl file defines the stylesheet of file
       http://mot-adms/sourceforge.net/xml-files/admst.xml 
-->
<!--
  $Id: admststyle.xsl 1053 2008-08-07 16:30:07Z r29173 $
-->
<!--
  $Log$
  Revision 1.10  2005/05/10 08:31:39  r29173
  improved documentation

  Revision 1.9  2005/04/04 14:48:37  r29173
  improved web presentation

  Revision 1.8  2005/03/24 17:03:27  r29173
  migration to adms-2.x.x: fixed bugs in admstpath parser

  Revision 1.7  2005/03/22 11:30:18  r29173
  migration to adms-2.x.x: clean-up xml constructs

  Revision 1.6  2005/03/17 08:26:28  r29173
  migration to adms-2.x.x: move all href from absolute to relative values

  Revision 1.5  2005/03/14 17:34:20  r29173
  migration to adms-2.x.x: start xpath coding; add documentation

  Revision 1.4  2005/02/28 09:22:12  r29173
  migration to adms-2.x.x: admst:size renamed admst:count

  Revision 1.3  2005/02/28 08:18:01  r29173
  migration to adms-2.x.x: table created

  Revision 1.2  2005/02/28 08:03:29  r29173
  migration to adms-2.x.x: admst:version required

  Revision 1.1  2005/02/25 11:46:17  r29173
  migration to adms-2.x.x: defines admst namespace

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:preserve-space elements="*"/>
<xsl:output method="html"/>
<xsl:template match="/">
<html xmlns:admst="http://mot-adms/sourceforge.net">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
    <title>admst.xsl: The admst Language - Reference Document</title>
    <meta name="generator" content="manual, see http://mot-adms.sourceforge.net/" />
    <link rel="stylesheet" type="text/css" href="./adms.css" />
  </head>
  <body>
  <h1 class="title">Reference Manual of admst-2 Language</h1>
  <p>
  <span class="freescale">A Freescale.com !n!t!at!ve</span>
  <span class="sourceforge">Supported by www.sourceforge.net</span>
  </p>
  <h1>Introduction</h1>
  <p>
  This document is the reference document of the admst language.
  admst is a subset of the xml language. Its DTD can be found at 
  <a href="./admst.dtd">admst.dtd</a>.
  admst is used by adms to transform verilog-ams source code into any kind of outputs
  (html documents, C code for spice simulators.) The primary application of admst is to create
  C code generators that generate from verilog-ams language ready-to-compile C code for some simulator-specific API.
  However its use can be extended to other applications like the automatic generation of html documentation. Most of the syntax used by admst has been borrowed from the xsl language.<br/>
  The command below shows how adms interprets admst scripts (myadmst.xml is an admst script and myverilogams.va 
  is a verilog-ams file):
  </p>
  <span class="shell_code">admsXml myverilogams.va -e myadmst.xml</span>
  <p>
  admsXml interprets the admst intructions found in file myadmst.xml and apply the instructions
  to the contents of the verilog-ams file myverilogams.va. It will produce some output files
  according to the admst transforms specified into myadmst.xml.
  </p>
  <h1>Example of admst script</h1>
  <pre class="admst_code">
  <![CDATA[
  1. <?xml version="1.0" encoding="ISO-8859-1"?>
  2. <!DOCTYPE admst PUBLIC "-//adms//DTD admst 2.0//-" "http://mot-adms.sourceforge.net/xml-files/adms/admst.dtd">
  3. <admst version="1.48.0" xmlns:admst="http://mot-adms.sourceforge.net/adms/admst.xml">
  4.  <admst:getenv name="HOME"/>
  5.  <admst:text format="HOME is %s\n"/>
  6. </admst>]]>
  </pre>
  <ul>
  <li>
    Line 1. is required.
     It is not used by admsXml but it helps xml tools to identify the contents of the admst file.
  </li>
  <li>
    Line 2. is required.
     It defines a link to the DTD of the admst language. The admst DTD is similar to a yacc grammar.
     It specifies the syntax of the admst language. 
     It is not used by adms.
     If program xmllint is installed on your system then the following command can check the validity of script
     myadmst.xml against DTD <a href="./admst.dtd">admst.dtd</a>:
  <p><span class="shell_code">
    xmllint admst.xml -noout -postvalid -noblanks
  </span></p>
  </li>
  <li>
    Line 3. and Line 6. are required.
      They delimit the declaration of the admst transforms.
  </li>
  <li>
    Line 4. illustrates the use of admst transform admst:getenv.
    admst:getenv has one attribute called <i>name</i>. Transform
    admst:getenv will look for the value of shell environement variable
    <i>HOME</i> and pushes its value to a internal stack created by admsXml.
  </li>
  <li>
    Line 5. illustrates the use of admst transform admst:text.
    admst:text has one attribute called <i>format</i>. Transform
    admst:text will print the text specified into attribute <i>format</i>. 
    Each occurence of <i>%s</i> will be replaced by the top value of the internal
    stack created by admsXml. 
  </li>
  </ul>
  <h1>List of admst Transforms</h1>
  <p>
   The list below provides a brief description of all possible admst transforms.
   Each admst transform has a set of attributes that modify or refine its behavior.
   An attribute is made of a name and a value: attrName=&quot;attrValue&quot;.
   The container of value attrValue can either be a scalar or a list.
   The data type of value attrValue can either be a basic type (integer, real, string, enumeration) or
   a reference to an adms element (module, node, branch, ...).
   adms uses attribute properties (datatypefamily, datatypename, size) to check dynamically the validity of attrValue.
   A star (&quot;*&quot;) means that attrValue can be any adms element or can be of any type.
  </p>
  <p>
  Examples:
  <ul>
    <li>attribute <i>version</i> of admst transform <i>admst</i> can only be a scalar string.</li>
    <li>attribute <i>select</i> of admst transform <i>admst:for-each</i> can only be a list of any adms elements.</li>
  </ul>
  </p>
  <table-wrap id="admst.xml.table">
  <label></label>
  <table border="1">
  <caption><p>admst transforms</p></caption>
  <thead>
    <tr>
      <td rowspan="2">name</td>
      <td colspan="4" style="text-align: center;">attributes</td>
    </tr>
    <tr>
      <td rowspan="2">name</td>
      <td>type</td>
      <td>size</td>
      <td>required</td>
    </tr>
  </thead>
  <tbody>
  <xsl:for-each select="admst/transform">
    <xsl:sort select="@name"/>
    <xsl:variable name="tname" select="@name"/>
    <xsl:variable name="size"
      select="count(attribute|attribute-implied|attribute-obsolete|attribute-warning|attribute-list|*/attribute|*/attribute-list)"/>
    <xsl:for-each select="attribute|attribute-implied|attribute-obsolete|attribute-warning|attribute-list|*/attribute|*/attribute-list">
      <tr>
        <xsl:if test="position()=1">
          <td>
            <xsl:attribute name="rowspan"><xsl:value-of select="$size" /></xsl:attribute> 
            <a>
              <xsl:attribute name="href">#howtouse:<xsl:value-of select="$tname"/></xsl:attribute> 
              <xsl:value-of select="$tname"/>
            </a>
          </td>
        </xsl:if>
        <td><xsl:value-of select="@name"/></td>
        <td>
          <xsl:choose>
            <xsl:when test="name(.)='attribute-obsolete'">
              <xsl:choose>
                <xsl:when test="@new">obsolete - see <xsl:value-of select="@new"/></xsl:when>
                <xsl:otherwise>obsolete - ignored</xsl:otherwise>
              </xsl:choose>
            </xsl:when>
            <xsl:otherwise><xsl:value-of select="@type"/></xsl:otherwise>
          </xsl:choose>
        </td>
        <td>
          <xsl:choose>
            <xsl:when test="name(.)='attribute-list'">list</xsl:when>
            <xsl:otherwise>scalar</xsl:otherwise>
          </xsl:choose>
        </td>
        <td>
          <xsl:choose>
            <xsl:when test="name(.)='attribute-warning'">warn</xsl:when>
            <xsl:when test="name(.)='attribute'">yes</xsl:when>
            <xsl:otherwise>no</xsl:otherwise>
          </xsl:choose>
        </td>
      </tr>
    </xsl:for-each>
  </xsl:for-each>
  </tbody>
  </table>
  </table-wrap>
  <!--xsl:for-each select="admst/transform">
    <xsl:sort select="@name"/>
    <h1><a>
      <xsl:attribute name="id">howtouse:<xsl:value-of select="@name"/>
      </xsl:attribute> 
      <xsl:value-of select="@name"/>
    </a></h1>
    <h2>Syntax</h2>
    <p>
    <span class="admst_transform">
      <xsl:text>&lt;</xsl:text>
      admst:<xsl:value-of select="@name"/>
      <xsl:for-each select="attribute">
        <xsl:sort select="@name"/>
        <xsl:text> </xsl:text>
        <xsl:if test="@required='no'">
          <xsl:text>[</xsl:text>
        </xsl:if>
        <xsl:value-of select="@name"/>
        <xsl:text>="..."</xsl:text>
        <xsl:if test="@required='no'">
          <xsl:text>]</xsl:text>
        </xsl:if>
      </xsl:for-each>
      <xsl:text>/&gt;</xsl:text>
    </span>
      <xsl:value-of select="syntax/text()"/>
    </p>
    <h2>Description</h2>
    <p>
      <p><xsl:value-of select="description/text()"/></p>
      <ul>
      <xsl:if test="not(count(attribute))">    
        <li>No attribute</li>
      </xsl:if>
      <xsl:for-each select="attribute">
        <li>
        <span class="admst_attribute"><xsl:value-of select="@name"/></span>:
        <xsl:value-of select="description/text()"/>
        </li>
      </xsl:for-each>
      </ul>
    </p>
    <h2>Example</h2>
    <p>
      <xsl:value-of select="example/text()"/>
    </p>
  </xsl:for-each-->
  <hr/>
  <p>
  This page has been automatically created by applying the xsl transform
  <a href="./admststyle.xsl">admststyle.xsl</a>
  to the xml data file
  <a href="./admst.xml">admst.xml</a>.
  <br/>
  The rendering of the page is specified by the css stylesheet
  <a href="./adms.css">adms.css</a>.
  You can get the full contents of data file 
  <a href="./admst.xml">admst.xml</a>
  by viewing the source of the document from your web browser.
  Please address any comments to <a href="mailto:r29173@users.sourceforge.net">r29173</a>.
  </p>
  </body>
</html>
</xsl:template>
</xsl:stylesheet>

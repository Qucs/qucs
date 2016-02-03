<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- 
       This xsl file defines the stylesheet of file
       http://mot-adms/sourceforge.net/xml-files/adms.xml 
-->
<!--
  $Id: admsstyle.xsl 1100 2008-10-02 15:16:44Z r29173 $
-->
<!--
  $Log$
  Revision 1.24  2006/09/04 08:51:05  r29173
  tons of simplifications

  Revision 1.23  2006/05/29 07:53:35  r29173
  improved display of stylesheet

  Revision 1.22  2005/11/25 14:15:25  r29173
  changed admstpath parser mechanisms

  Revision 1.21  2005/11/22 09:29:47  r29173
  added element comment

  Revision 1.20  2005/11/01 14:10:31  r29173
  removed sorting - keep same order than adms.xml

  Revision 1.19  2005/06/16 07:12:42  r29173
  debug memory allocation with valgrind

  Revision 1.18  2005/05/27 16:30:38  r29173
  moving code from xmlParserNew.c to xml - added subexpression

  Revision 1.17  2005/05/25 08:27:49  r29173
  moving code from  xmlParserNew.c to xml

  Revision 1.16  2005/04/22 06:54:43  r29173
  fixed bug - admstpath:position handling

  Revision 1.15  2005/04/15 08:38:37  r29173
  renamed png files

  Revision 1.14  2005/04/14 13:31:20  r29173
  improved documentation

  Revision 1.13  2005/04/14 10:21:31  r29173
  improved documentation

  Revision 1.12  2005/04/14 10:14:54  r29173
  improved documentation

  Revision 1.11  2005/04/04 14:48:37  r29173
  improved web presentation

  Revision 1.10  2005/03/22 11:30:18  r29173
  migration to adms-2.x.x: clean-up xml constructs

  Revision 1.9  2005/03/17 08:26:28  r29173
  migration to adms-2.x.x: move all href from absolute to relative values

  Revision 1.8  2005/03/16 17:37:47  r29173
  migration to adms-2.x.x: cosmetics

  Revision 1.7  2005/03/16 17:31:24  r29173
  migration to adms-2.x.x: small cosmetic

  Revision 1.6  2005/03/16 16:29:01  r29173
  migration to adms-2.x.x: start xpath coding; add documentation

  Revision 1.5  2005/03/14 17:58:48  r29173
  migration to adms-2.x.x: improved documentation

  Revision 1.4  2005/03/14 17:34:20  r29173
  migration to adms-2.x.x: start xpath coding; add documentation

  Revision 1.3  2005/02/25 12:49:35  r29173
  migration to adms-2.x.x: renamed admst transforms

-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>

<xsl:template match="/">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
    <title>adms.xsl: The adms Data Tree - Reference Document</title>
    <meta name="generator" content="manual, see http://mot-adms.sourceforge.net/"/>
    <link rel="stylesheet" type="text/css" href="./adms.css"/>
    <script type="text/javascript">
    <![CDATA[
     function expandNode(branch)
     {
        var objBranch=document.getElementById(branch).style;
        objBranch.display="block";
     }
     function switchNodeCollaping(branch)
     {
        var objBranch=document.getElementById(branch).style;
        if(objBranch.display=="block")
           objBranch.display="none";
        else
           objBranch.display="block";
     }
    ]]>
    </script>
  </head>
  <body>
  <p>This section gives a tree view of the different internal data structure created by adms after parsing an input file.</p>
  <p>Symbols used:</p>
  <ul>
  <li><img src="images/adms-element.png"/>: adms element</li>
  <li><img src="images/adms-uid.png"/> property (constructor)</li>
  <li><img src="images/adms-scalar.png"/>: property (scalar)</li>
  <li><img src="images/adms-list.png"/> property (list)</li>
  </ul>
  <p>
  Clicking on a node will expand it.
  </p>
  <p>
  <xsl:for-each select="adms/element[@name='adms']">
    <xsl:apply-templates select="."/>
  </xsl:for-each>
  </p>
<!--footer-->
  <hr/>
  <p>
  This page has been automatically created by applying the xsl transform
  <a href="./admsstyle.xsl">admsstyle.xsl</a>
  to the xml data file
  <a href="./adms.xml">adms.xml</a>.
  <br/>
  The rendering of the page is specified by the css stylesheet
  <a href="./adms.css">adms.css</a>.
  You can get the full contents of data file 
  <a href="./adms.xml">adms.xml</a>
  by viewing the source of the document from your web browser.
  Please address any comments to <a href="mailto:r29173@users.sourceforge.net">r29173</a>.
  </p>
  </body>
</html>
</xsl:template>
<xsl:template match="element">
  <span class="trigger">
    <xsl:attribute name="id">root<xsl:value-of select="@name"/></xsl:attribute>
    <xsl:attribute name="onClick">switchNodeCollaping('<xsl:value-of select="@name"/>');</xsl:attribute>
    <img src="images/adms-element.png"/>
    <xsl:choose>
      <xsl:when test="element"><b><xsl:value-of select="@name"/></b></xsl:when>
      <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise>
    </xsl:choose>
    <span class="info"><xsl:value-of select="@info"/> (uid=&quot;<xsl:value-of select="uid/@format"/>&quot;)</span>
    <br/>
  </span>
  <span class="branch">
    <xsl:attribute name="id"><xsl:value-of select="@name"/></xsl:attribute>
    <xsl:if test="@name='adms'"><xsl:attribute name="style">display:block</xsl:attribute></xsl:if>
    <xsl:apply-templates select="uid/scalar|uid/list|scalar|list|warning|comment|element|alias"/>
  </span>
</xsl:template>
<xsl:template match="scalar|list">
  <span class="trigger">
    <xsl:choose>
      <xsl:when test="name(..)='uid'">
        <img> <xsl:attribute name="src">images/adms-uid.png</xsl:attribute></img>
        <xsl:variable name="id" select="concat(../../@name,@name)"/>
      </xsl:when>
      <xsl:otherwise>
        <img> <xsl:attribute name="src">images/adms-<xsl:value-of select="name(.)"/>.png</xsl:attribute> </img>
        <xsl:variable name="id" select="concat(../@name,@name)"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="@datatypename='basicenumeration' or @datatypename='enumeration'">
        <span>
        <xsl:variable name="id" select="concat(../../@name,@name)"/>
        <xsl:attribute name="onClick">switchNodeCollaping('<xsl:value-of select="$id"/>');</xsl:attribute>
          (<xsl:value-of select="@datatypename"/>)
          <xsl:value-of select="@name"/>
        <span class="info"><xsl:value-of select="@info"/></span>
        <span class="branch">
          <xsl:attribute name="id"><xsl:value-of select="$id"/></xsl:attribute>
          <xsl:for-each select="evalue">
            <img src="images/adms-enumeration.png"/>
            <span><xsl:value-of select="@name"/></span>
            <span class="info"><xsl:value-of select="@info"/></span>
            <br/>
          </xsl:for-each>
        </span>
        </span>
      </xsl:when>
      <xsl:when test="@datatypefamily='reference' and @datatypename!='*'">
        <span>
          <xsl:attribute name="onClick">expandNode('<xsl:value-of select="@datatypename"/>');</xsl:attribute>
          <a>
            <xsl:attribute name="href">#root<xsl:value-of select="@datatypename"/></xsl:attribute>
            (<xsl:value-of select="@datatypename"/>)
          </a>
          <xsl:value-of select="@name"/>
        </span>
        <span class="info"><xsl:value-of select="@info"/></span>
      </xsl:when>
      <xsl:otherwise>
        <span>
          (<xsl:value-of select="@datatypename"/>)
          <xsl:value-of select="@name"/>
        </span>
        <span class="info"><xsl:value-of select="@info"/></span>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="@format"><span class="info">(uid=&quot;<xsl:value-of select="@format"/>&quot;)</span></xsl:if>
  </span>
</xsl:template>
<xsl:template match="warning">
  <span class="comment"><b><xsl:value-of select="text()"/></b></span>
</xsl:template>
<xsl:template match="comment">
  <span class="comment"><xsl:value-of select="text()"/></span>
</xsl:template>
<xsl:template match="alias">
  <span class="trigger">
    <img src="images/adms-alias.png"/>
    (alias) <b><xsl:value-of select="../@name"/>/<xsl:value-of select="@name"/></b>
    same as <b><xsl:value-of select="../@name"/>/<xsl:value-of select="@element"/>/<xsl:value-of select="@attribute"/></b>
  </span>
</xsl:template>
<xsl:template match="text()"></xsl:template>
<xsl:template match="comment()"></xsl:template>


</xsl:stylesheet>

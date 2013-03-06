<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- 
       This xsl file defines the stylesheet of file
       http://mot-adms/sourceforge.net/xml-files/adms.xml 
-->
<!--
  $Id: admstpathstyle.xsl 1003 2008-05-15 17:25:30Z r29173 $
-->
<!--
  $Log$
  Revision 1.18  2006/09/20 09:26:35  r29173
  changed construction of probe (test on mtj/moscap3 simkit fail)

  Revision 1.17  2006/09/14 11:23:20  r29173
  migration to www.antlr.org tools

  Revision 1.16  2006/09/13 16:13:19  r29173
  started migration to www.antlr.org tools

  Revision 1.15  2005/12/08 10:11:20  r29173
  fixed bad index in mytext array - replace indexes by names

  Revision 1.14  2005/12/05 17:15:01  r29173
  improved admst parser (no tokentext for constant terminals)

  Revision 1.13  2005/11/30 13:58:17  r29173
  added nodes maincode and headercode

  Revision 1.12  2005/04/04 15:23:52  r29173
  improved web presentation

  Revision 1.11  2005/04/04 14:48:37  r29173
  improved web presentation

  Revision 1.10  2005/03/31 18:12:01  r29173
  migration to adms-2.x.x: converted to adms2 syntax

  Revision 1.9  2005/03/24 17:39:02  r29173
  migration to adms-2.x.x: moved c-code from perl to xml

  Revision 1.8  2005/03/24 17:03:27  r29173
  migration to adms-2.x.x: fixed bugs in admstpath parser

  Revision 1.7  2005/03/23 15:16:54  r29173
  add button displaySemantic

  Revision 1.6  2005/03/23 13:38:33  r29173
  migration to adms-2.x.x: added xpath parser

  Revision 1.5  2005/03/22 11:30:18  r29173
  migration to adms-2.x.x: clean-up xml constructs

  Revision 1.4  2005/03/17 11:46:04  r29173
  migration to adms-2.x.x: cosmetics

  Revision 1.3  2005/03/17 08:26:28  r29173
  migration to adms-2.x.x: move all href from absolute to relative values

  Revision 1.2  2005/03/16 16:35:32  r29173
  migration to adms-2.x.x: fix spelling

  Revision 1.1  2005/03/16 16:17:17  r29173
  migration to adms-2.x.x: build prerequisites for admst path

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
<html xmlns:admstpath="http://mot-adms/sourceforge.net">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
    <title>admstpath.xml: The admst Path Syntax - Reference Document</title>
    <meta name="generator" content="manual, see http://mot-adms.sourceforge.net/" />
    <link rel="stylesheet" type="text/css" href="./adms.css" />
    <script type="text/javascript">
    <![CDATA[
      function displaySemantic()
      {
         var Obj = document.getElementsByTagName("button");
         for(var i=0;i<(Obj.length);i++)
         {
           if(Obj[i].className=="callbackVisible")
             Obj[i].className="callbackHidden"
           else if(Obj[i].className=="callbackHidden")
             Obj[i].className="callbackVisible"
         }
      }
      function showSemanticCCode(arg)
      {
         var myObj = document.getElementById(arg);
         if(!myObj)
         {
           alert("code "+arg+" not found");
           return;
         }
         var myString = new String(myObj.firstChild.data);
         confirm(arg+"\n"+myString);
      }
    ]]>
    </script>
  </head>
  <body>
  <h1>Introduction</h1>
  <p>
  This section gives the details of the yacc grammar used to build the admst path parser.
  The admst path is very similar to the xml xpath language.
  In future releases of adms we plan to substitute the admst path by the xml xpath language.
  Lot of development is on going on importing the xpath technology to adms.
  </p>
  <h1>Example</h1>
  <p>
  Some examples of valid admst paths
  </p>
  <table border="1">
  <label></label>
  <caption><p>admst path: examples</p></caption>
  <thead>
    <tr>
      <td>admst path</td>
      <td>description</td>
    </tr>
  </thead>
  <tbody>
  <tr>
  <td><![CDATA[/module]]></td>
  <td>select all modules</td>
  </tr>
  <tr>
  <td><![CDATA[node[location='external']]]></td>
  <td>select nodes the position of which is external - in other words select terminals</td>
  </tr>
  <tr>
  <td><![CDATA[node[location='external' or name='gate']]]></td>
  <td>select terminal the name of which is 'gate'</td>
  </tr>
  </tbody>
  </table>
  <h1>admst path: Grammar Definition</h1>
  <p>
  The admst path grammar description is very simple.
  This is due to the limitations of the method applied to build the parser into adms
  (predicative parser).
  But it is good enough for most of the current applications that use adms. 
  </p>
  <button onClick="displaySemantic()">display semantic</button>
  <p>
  <xsl:for-each select="/admstpath/yacc/code">
    <pre style="display:none;">
      <xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
      <xsl:value-of select="text()"/>
    </pre>
  </xsl:for-each>
  <xsl:for-each select="/admstpath/regexp">
    <pre style="display:none;">
      <xsl:attribute name="id">
        <xsl:value-of select="@id"/>
      </xsl:attribute>
      <xsl:value-of select="text()"/>
    </pre>
  </xsl:for-each>
  <ul class="freegrammar_productionset">
  <xsl:for-each select="/admstpath/yacc/productionset">
  <li class="freegrammar_productionset">
    <xsl:value-of select="@id"/>
    <ul>
    <xsl:for-each select="production">
    <li class="productionset_production">
      <span><xsl:value-of select="@id"/><xsl:text>. </xsl:text></span>
      <xsl:for-each select="item">
        <span class="freegrammar_productionset_production_item">
        <xsl:variable name="id" select="@linkto"/>
        <xsl:choose>
          <xsl:when test="/admstpath/lex/constant[@id=$id]">
            <a href="#{generate-id(/admstpath/lex/constant[@id=$id])}">
            <span class="production_constant"><xsl:value-of select="/admstpath/lex/constant[@id=$id]/@value"/></span>
            </a>
          </xsl:when>
          <xsl:when test="/admstpath/lex/regexp[contains(@id,$id)]">
            <a href="#{generate-id(/admstpath/lex/regexp[contains(@id,$id)])}">
              <span class="production_regexp"><xsl:value-of select="$id"/></span>
            </a>
          </xsl:when>
          <xsl:otherwise>
            <span class="production_productionset"><xsl:value-of select="$id"/></span>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:text> </xsl:text>
        </span>
        <xsl:for-each select="callback">
          <button class="callbackVisible">
            <xsl:attribute name="onClick">
              showSemanticCCode('<xsl:value-of select="@id"/>')
            </xsl:attribute>
            <xsl:value-of select="@id"/>
          </button>
        </xsl:for-each>
        <xsl:text> </xsl:text>
      </xsl:for-each>
    </li>
    </xsl:for-each>
    <xsl:for-each select="empty">
      <li class="productionset_production">
        <span class="production_empty">&#216;</span>
        <xsl:text> </xsl:text>
        <xsl:for-each select="callback">
          <button class="callbackVisible">
            <xsl:attribute name="onClick">
              showSemanticCCode('<xsl:value-of select="@id"/>')
            </xsl:attribute>
            <xsl:value-of select="@id"/>
          </button>
        </xsl:for-each>
      </li>
    </xsl:for-each>
    </ul>
  </li>
  </xsl:for-each>
  </ul>
  </p>
  <h1>admst path: lexical terminals</h1>
  <table-wrap id="adms.xml.table">
  <label></label>
  <table border="1">
  <caption><p>admst path: lexical terminals</p></caption>
  <thead>
    <tr>
      <td>terminal</td>
      <td>regexp</td>
      <td>description</td>
    </tr>
  </thead>
  <tbody>
  <xsl:for-each select="/admstpath/lex/constant">
    <tr>
      <td>
        <a name="{generate-id(.)}"><xsl:value-of select="@id"/></a>
      </td>
      <td style="text-align: center;">
        <xsl:variable name="regexp" select="@value"/>
        <xsl:if test="/admstpath/regexp[@id=$regexp]">
          <span>
            <xsl:attribute name="onClick">showSemanticCCode('<xsl:value-of select="@value"/>')</xsl:attribute>
            <xsl:attribute name="style">color:blue;</xsl:attribute>
            <xsl:value-of select="$regexp"/>
          </span>
        </xsl:if>
        <xsl:if test="not(/admstpath/regexp[@id=$regexp])">
          <xsl:value-of select="$regexp"/>
        </xsl:if>
      </td>
      <td><xsl:value-of select="text()"/></td>
    </tr>
  </xsl:for-each>
  <xsl:for-each select="/admstpath/lex/regexp">
    <tr>
      <td>
        <a name="{generate-id(.)}"><xsl:value-of select="@id"/></a>
      </td>
      <td style="text-align: center;">
        <xsl:variable name="regexp" select="@value"/>
        <xsl:if test="/admstpath/regexp[@id=$regexp]">
          <span>
            <xsl:attribute name="onClick">showSemanticCCode('<xsl:value-of select="@value"/>')</xsl:attribute>
            <xsl:attribute name="style">color:blue;</xsl:attribute>
            <xsl:value-of select="$regexp"/>
          </span>
        </xsl:if>
        <xsl:if test="not(/admstpath/regexp[@id=$regexp])">
          <xsl:value-of select="$regexp"/>
        </xsl:if>
      </td>
      <td><xsl:value-of select="text()"/></td>
    </tr>
  </xsl:for-each>
  </tbody>
  </table>
  </table-wrap>
  <hr/>
  <p>
  This page has been automatically created by applying the xsl transform
  <a href="./admstpathstyle.xsl">admstpathstyle.xsl</a>
  to the xml data file
  <a href="./admstpath.xml">admstpath.xml</a>.
  <br/>
  The rendering of the page is specified by the css stylesheet
  <a href="./adms.css">adms.css</a>.
  You can get the full contents of data file 
  <a href="./adms.xml">admstpath.xml</a>
  by viewing the source of the document from your web browser.
  Please address any comments to <a href="mailto:r29173@users.sourceforge.net">r29173</a>.
  </p>
  </body>
</html>
</xsl:template>
</xsl:stylesheet>

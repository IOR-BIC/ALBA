<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:str="http://exslt.org/strings" extension-element-prefixes="str">
<xsl:output method="html" encoding="UTF-8"/>
<xsl:template match="/">
<html>
<!-- **** layout stylesheet **** -->
<link rel="stylesheet" type="text/css" href="../html/Styles/style.css" />

<!-- **** colour scheme stylesheet **** -->
<link rel="stylesheet" type="text/css" href="../html/Styles/colour.css" />
<head><title>cppcheck Report</title>
</head>
<body>
<div id="main">
    <div id="links">
      <!-- **** INSERT LINKS HERE **** -->
      <!--a href="#">another link</a> | <a href="#">another link</a> | <a href="#">another link</a> | <a href="#">another link</a-->
    @@@_PUBLISH_DATE_@@@
    </div>
    <div id="logo"><h1>MAF Quality Assurance</h1></div>
    <div id="content">
      <div id="menu">
        <ul>
          <li><a href="../html/index.html">Introduction</a></li>
          <!-- **** INSERT ADDITIONAL EXTERNAL TOOLS REPORT_LINK HERE **** -->
          @@@_EXTERNAL_TOOLS_REPORT_@@@
        </ul>
      </div>
      <div id="column1"> 
<div class="sidebaritem"></div> 
<div class="sidebaritem"> 
<div class="sbihead"><h1>Cyc.Complexity</h1></div> 
<div class="sbilinks"><ul><div class="maf3LinkInsertion"> 
    @@@_EXTERNAL_TOOLS_LINKS_@@@
</div></ul></div> 
</div></div>

<div class="sidebaritem"></div> 

<div id="column2"><div class="maf3RuleInsertion"><div id="wrapper-container"><div id="container"> 
<div id="header" class="clear"><h1>Cyclomatic Complexity</h1></div> 
<div id="subheader" class="clear"><h3> Each file is evaluated, checking the Cyclomatic Complexity.</h3></div>
<div id="contentTable" align="center">
 <table id="listing">
  <thead>
	<tr>
	   <th width="80%" align="center"><b>File</b></th>
 	   <th width="20" align="center"><b>Cyclomatic Complexity</b></th>
	</tr>
  </thead> 
  <tbody>
	<xsl:for-each select="CCCC_Project/procedural_summary/module">
       <xsl:choose>
        <xsl:when test="position() mod 2 != 1">
        <tr class="odd">
         <td width="50%"><b><xsl:value-of select="name"/></b></td>
         <xsl:for-each select="McCabes_cyclomatic_complexity">
         <td width="50%"><b><xsl:value-of select="@value"/></b></td>
         </xsl:for-each>
        </tr>
        </xsl:when>
        <xsl:otherwise>
         <tr class="even">
         <td width="50%"><b><xsl:value-of select="name"/></b></td>
         <xsl:for-each select="McCabes_cyclomatic_complexity">
         <td width="50%"><b><xsl:value-of select="@value"/></b></td>
         </xsl:for-each>
        </tr>
       </xsl:otherwise>
      </xsl:choose>
     </xsl:for-each>

  </tbody>
 </table>
</div>
</div>
</div>
</div>
</div>
</div>
<div id="footer"> 
      copyright 2010 SCS | <a href="http://www.biomedtown.org/biomed_town/MAF/MAF3%20Floor/">MAF3</a> | <a href="http://validator.w3.org/check?uri=referer">XHTML 1.1</a> | <a href="http://jigsaw.w3.org/css-validator/check/referer">CSS</a> 
</div>
</div>
</body>
</html>
</xsl:template>
</xsl:stylesheet>
<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:param name="dir"/>
  <xsl:template match="/">
    <HTML>
      <HEAD>
        <TITLE>
          Wrong files list
        </TITLE>
      </HEAD>
      <BODY>
        <xsl:for-each select="root/Directory">
          <xsl:if test="$dir = @DirName">
            <h1>
              Wrong files in <xsl:value-of select="$dir"/> are shown in red
            </h1>
            <p>
              <xsl:for-each select="WrongFiles">
                <p><font color="red">
                  <xsl:value-of select="@FileName"/>
                </font></p>
              </xsl:for-each>
              <xsl:for-each select="GoodFiles">
                <p>
                  <xsl:value-of select="@FileName"/>
                </p>
              </xsl:for-each>
            </p>
          </xsl:if>
        </xsl:for-each>
        <p>
          <a HREF="FileNamingReport.html">Home</a>
        </p>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>
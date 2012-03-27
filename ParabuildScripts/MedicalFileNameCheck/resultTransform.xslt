<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/">
    <HTML>
      <HEAD>
        <TITLE>
          File Name convention check
        </TITLE>
      </HEAD>
      <BODY>
        <h1>
          Report Result: <xsl:value-of select="//root/ReportDate"/>
        </h1>
        <table border="1">
          <tr>
            <th style="text-align:left">Directory</th>
            <th>Percentage</th>
          </tr>
          <xsl:for-each select="root/Directory">
            <tr>
              <th style="text-align:left">
                <a href="{@DirName}.html">
                  <xsl:value-of select="@DirName"/>
                </a>
              </th>
              <td style="text-align:right">
                <xsl:value-of select="@Percentage"/>
              </td>
            </tr>
          </xsl:for-each>
        </table>
        <p>
          Computation Time = <xsl:value-of select="root/ComputationTime"/>
        </p>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>
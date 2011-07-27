		
        <div id="wrapper-container">
			<div id="container">
				<div id="header" class="clear">
					<h1>Guide Creator Rule</h1>
				</div>
				<div id="subheader" class="clear">
					<h3>
						This rule extrapolates comments from unit tests in order to create online documentation,
                        for a quick "How to use this class" tutorial.
					</h3>
                    <h4><br/>Link to <a href="http://www.biomedtown.org/biomed_town/MAF/MAF3%20Floor/documentation/qa/maf3-automatic-guide-creator-script/">Guide Creator rule</a></h4>
				</div>
				<div id="contentTable">
					<table id="listing">
						<tbody>
                        <xsl:for-each select="root/results/item">
                          <xsl:choose>
                           <xsl:when test="position() mod 2 != 1">
                           <tr class="odd">
                           <td width="70%"><b><a name="prova"><xsl:value-of select="title"/></a></b><br/><br/>
				           <font size="2"><b><xsl:value-of select="description"/></b></font></td>

                           <xsl:for-each select="snippets/snippet">
                             <tr class="odd">
                             <td width="70%">
                             <font size="1">
                             <xsl:call-template name="break">
	                            <xsl:with-param name="text" select="." />
                             </xsl:call-template>  
                             </font>
                             </td>
                             </tr>
                           </xsl:for-each>
                           </tr>

                           </xsl:when>
                           <xsl:otherwise>
                            <tr class="even">
                            <td width="70%"><b><a name="prova"><xsl:value-of select="title"/></a></b><br/><br/>
				            <font size="2"><b><xsl:value-of select="description"/></b></font></td>
                            <xsl:for-each select="snippets/snippet">
                             <tr class="even">
                             <td width="70%">
                             <font size="1">
                             <xsl:call-template name="break">
	                            <xsl:with-param name="text" select="." />
                             </xsl:call-template>                             
                             </font>
                             </td>
                             </tr>
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

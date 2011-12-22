		<div id="wrapper-container">
			<div id="container">
				<div id="header" class="clear">
					<h1>Sources Comment Rule</h1>
				</div>
				<div id="subheader" class="clear">
					<h3>
						Each source file is evaluated, check if gui components have tooltips or not
					</h3>
				</div>
				<div id="contentTable">
					<table id="listing">
						<thead>
						<tr>
							<th>class</th>
							<th>line number</th>
						</tr>
						</thead>
						<tbody>
						<xsl:for-each select="root/results/item">
                          <xsl:choose>
                           <xsl:when test="position() mod 2 != 1">
                            <tr class="odd">
                            <td width="50%"><b><xsl:value-of select="class"/></b></td>
                            <td width="50%"><xsl:value-of select="line"/></td>
                            </tr>
                           </xsl:when>
                           <xsl:otherwise>
                            <tr class="even">
                            <td width="50%"><b><xsl:value-of select="class"/></b></td>
                            <td width="50%"><xsl:value-of select="line"/></td>
                            </tr>
                           </xsl:otherwise>
                         </xsl:choose>
                      </xsl:for-each>
						</tbody>
					</table>
				</div>
				<div id="subheader" class="clear">
					<h3>
                      GUI Tooltipped : <xsl:value-of select="root/results/percentage"/>
					</h3>
				</div>
			</div>
		</div>

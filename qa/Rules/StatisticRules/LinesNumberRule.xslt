		<div id="wrapper-container">
			<div id="container">
				<div id="header" class="clear">
					<h1>Number Of Lines</h1>
				</div>
				<div id="subheader" class="clear">
					<h3>
						Each file is evaluated, checking the number of line (not empty) for each class.
					</h3>
				</div>
				<div id="contentTable">
					<table id="listing">
						<thead>
						<tr>
							<th>file</th>
							<th>Number of Lines</th>
						</tr>
						</thead>
						<tbody>
						<xsl:for-each select="root/results/item">
                          <xsl:choose>
                           <xsl:when test="position() mod 2 != 1">
                            <tr class="odd">
                            <td width="50%"><b><xsl:value-of select="file"/></b></td>
                            <td width="50%"><xsl:value-of select="linesNumber"/></td>
                            </tr>
                           </xsl:when>
                           <xsl:otherwise>
                            <tr class="even">
                            <td width="50%"><b><xsl:value-of select="file"/></b></td>
                            <td width="50%"><xsl:value-of select="linesNumber"/></td>
                            </tr>
                           </xsl:otherwise>
                         </xsl:choose>
                      </xsl:for-each>
						</tbody>
					</table>
				</div>
				<div id="subheader" class="clear">
					<h3>
                        Total Number Of Lines: <xsl:value-of select='sum(//linesNumber)'/>
					</h3>
                    <h3>
                        Total Number Of Files: <xsl:value-of select='count(//item)'/>
					</h3>
				</div>
			</div>
		</div>

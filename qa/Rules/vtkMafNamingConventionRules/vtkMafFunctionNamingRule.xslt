		<div id="wrapper-container">
			<div id="container">
				<div id="header" class="clear">
					<h1>Function Naming Rule</h1>
				</div>
				<div id="subheader" class="clear">
					<h3>
						This rule checks if, inside a class, all functions respect the naming convention rule.
                        If not the error is showed in the following table, giving information on class, method name, and line
                        of source file.
					</h3>
                    <h4><br/>Link to <a href="http://www.biomedtown.org/biomed_town/MAF/MAF3%20Floor/documentation/conventions/">Coding Conventions</a></h4>
				</div>
				<div id="contentTable">
					<table id="listing">
						<thead>
						<tr>
							<th>Class</th>
                            <th>Method</th>
                            <th>Line</th>
						</tr>
						</thead>
						<tbody>
						<xsl:for-each select="root/results/item">
                          <xsl:choose>
                           <xsl:when test="position() mod 2 != 1">
                            <tr class="odd">
                            <td width="40%"><b><xsl:value-of select="class"/></b></td>
				            <td width="40%"><xsl:value-of select="function"/></td>
				            <td width="20%"><xsl:value-of select="line"/></td>
                            </tr>
                           </xsl:when>
                           <xsl:otherwise>
                            <tr class="even">
                            <td width="40%"><b><xsl:value-of select="class"/></b></td>
				            <td width="40%"><xsl:value-of select="function"/></td>
				            <td width="20%"><xsl:value-of select="line"/></td>
                            </tr>
                           </xsl:otherwise>
                         </xsl:choose>
                      </xsl:for-each>
						</tbody>
					</table>
				</div>
				<div id="subheader" class="clear">
                <h3>Classes with good funtion naming: <xsl:value-of select="root/results/percentage"/></h3>
				</div>
			</div>
		</div>

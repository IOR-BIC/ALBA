		<div id="wrapper-container">
			<div id="container">
				<div id="header" class="clear">
					<h1>Sources Comment Rule</h1>
				</div>
				<div id="subheader" class="clear">
					<h3>
						Each source file is evaluated, checking the ratio between number of commented lines on total number of lines.
                        If the value, expressed as percentage, falls inside a range of 20% - 40%, the class is considered well commented,
                        otherwise the file is reported in the following table with the current percentage.
					</h3>
                    <h4><br/>Link to <a href="http://www.biomedtown.org/biomed_town/MAF/MAF3%20Floor/documentation/conventions/">Coding Conventions</a></h4>
				</div>
				<div id="contentTable">
					<table id="listing">
						<thead>
						<tr>
							<th>class</th>
							<th>comment percentage</th>
						</tr>
						</thead>
						<tbody>
						<xsl:for-each select="root/results/item">
                          <xsl:choose>
                           <xsl:when test="position() mod 2 != 1">
                            <tr class="odd">
                            <td width="50%"><b><xsl:value-of select="class"/></b></td>
                            <td width="50%"><xsl:value-of select="commentPercentage"/></td>
                            </tr>
                           </xsl:when>
                           <xsl:otherwise>
                            <tr class="even">
                            <td width="50%"><b><xsl:value-of select="class"/></b></td>
                            <td width="50%"><xsl:value-of select="commentPercentage"/></td>
                            </tr>
                           </xsl:otherwise>
                         </xsl:choose>
                      </xsl:for-each>
						</tbody>
					</table>
				</div>
				<div id="subheader" class="clear">
					<h3>
                      Good commented classes : <xsl:value-of select="root/results/percentage"/>
					</h3>
				</div>
			</div>
		</div>

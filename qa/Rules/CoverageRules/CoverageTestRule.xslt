		<div id="wrapper-container">
			<div id="container">
				<div id="header" class="clear">
					<h1>Coverage Test Rule</h1>
				</div>
				<div id="subheader" class="clear">
					<h3>
						Check the ratio between number of Tests and total number of Classes,
                        defined inside the framework. The following table shows the classes without test.
					</h3>
                    <h4><br/>Link to <a href="http://www.biomedtown.org/biomed_town/MAF/MAF3%20Floor/documentation/conventions/">Coding Conventions</a></h4>
				</div>
				<div id="contentTable">
					<table id="listing">
						<thead>
						<tr>
							<th>Test Class</th>
						</tr>
						</thead>
						<tbody>
						<xsl:for-each select="root/results/item">
                          <xsl:choose>
                           <xsl:when test="position() mod 2 != 1">
                            <tr class="odd">
                            <td width="100%"><b><xsl:value-of select="class"/></b></td>
                            </tr>
                           </xsl:when>
                           <xsl:otherwise>
                            <tr class="even">
                            <td width="100%"><b><xsl:value-of select="class"/></b></td>
                            </tr>
                           </xsl:otherwise>
                         </xsl:choose>
                      </xsl:for-each>
						</tbody>
					</table>
				</div>
				<div id="subheader" class="clear">
                <h3>(Classes with Test)/(All Classes)<xsl:value-of select="root/results/percentageCoverage"/></h3>
				</div>
			</div>
		</div>

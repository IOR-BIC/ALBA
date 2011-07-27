        <div id="wrapper-container">
			<div id="container">
				<div id="header" class="clear">
					<h1>Class Methods Documentation Rule</h1>
				</div>
				<div id="subheader" class="clear">
					<h3>
						This rule checks, in particular, if the each method of a class has brief documentation.
                        If not, the erroneus method is inserted in the table with information about the class.  
					</h3>
                    <h4><br/>Link to <a href="http://www.biomedtown.org/biomed_town/MAF/MAF3%20Floor/documentation/conventions/">Coding Conventions</a></h4>
				</div>
				<div id="contentTable">
					<table id="listing">
						<thead>
						<tr>
							<th>Class</th>
                            <th>Method</th>
						</tr>
						</thead>
						<tbody>
						<xsl:for-each select="root/results/item">
                          <xsl:choose>
                           <xsl:when test="position() mod 2 != 1">
                            <tr class="odd">
                            <td width="50%"><b><xsl:value-of select="class"/></b></td>
                            <td width="50%"><b><xsl:value-of select="function"/></b></td>
                            </tr>
                           </xsl:when>
                           <xsl:otherwise>
                            <tr class="even">
                            <td width="50%"><b><xsl:value-of select="class"/></b></td>
                            <td width="50%"><b><xsl:value-of select="function"/></b></td>
                            </tr>
                           </xsl:otherwise>
                         </xsl:choose>
                      </xsl:for-each>
						</tbody>
					</table>
				</div>
				<div id="subheader" class="clear">
                <h3>Classes with good method documentation: <xsl:value-of select="root/results/percentage"/></h3>
				</div>
			</div>
		</div>


		<div id="wrapper-container">
			<div id="container">
				<div id="header" class="clear">
					<h1>One class inside header file Rule</h1>
				</div>
				<div id="subheader" class="clear">
					<h3>
						Check if each class is contained in unique header file.
                        If more than one class will be located in a header it will be generate an error
                        for each class inside it.
					</h3>
                    <h4><br/>Link to <a href="http://www.biomedtown.org/biomed_town/MAF/MAF3%20Floor/documentation/conventions/">Coding Conventions</a></h4>
				</div>
				<div id="contentTable">
					<table id="listing">
						<thead>
						<tr>
							<th>Class</th>
                            <th>Other Classes</th>
                            <th>Include File</th>
						</tr>
						</thead>
						<tbody>
						<xsl:for-each select="root/results/item">
                          <xsl:choose>
                           <xsl:when test="position() mod 2 != 1">
                            <tr class="odd">
                            <td width="100%"><b><xsl:value-of select="class"/></b></td>
                            <td width="100%"><b><xsl:value-of select="otherClasses"/></b></td>
                            <td width="100%"><b><xsl:value-of select="includeFile"/></b></td>
                            </tr>
                           </xsl:when>
                           <xsl:otherwise>
                            <tr class="even">
                            <td width="100%"><b><xsl:value-of select="class"/></b></td>
                            <td width="100%"><b><xsl:value-of select="otherClasses"/></b></td>
                            <td width="100%"><b><xsl:value-of select="includeFile"/></b></td>
                            </tr>
                           </xsl:otherwise>
                         </xsl:choose>
                      </xsl:for-each>
						</tbody>
					</table>
				</div>
				<div id="subheader" class="clear">
                <h3>Classes inside unique header: <xsl:value-of select="root/results/percentage"/></h3>
				</div>
			</div>
		</div>

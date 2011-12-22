		<div id="wrapper-container">
			<div id="container">
				<div id="header" class="clear">
					<h1>License Header</h1>
				</div>
				<div id="subheader" class="clear">
					<h3>
						Check if each file contains in the first 10 lines the header with the link to the <a href="http://tiny.cc/QXJ4D">license</a>
					</h3>
				</div>
				<div id="contentTable">
					<table id="listing">
						<thead>
						<tr>
							<th>file</th>
						</tr>
						</thead>
						<tbody>
						<xsl:for-each select="root/results/item">
                          <xsl:choose>
                           <xsl:when test="position() mod 2 != 1">
                            <tr class="odd">
                            <td width="50%"><b><xsl:value-of select="file"/></b></td>
                            </tr>
                           </xsl:when>
                           <xsl:otherwise>
                            <tr class="even">
                            <td width="50%"><b><xsl:value-of select="file"/></b></td>
                            </tr>
                           </xsl:otherwise>
                         </xsl:choose>
                      </xsl:for-each>
						</tbody>
					</table>
				</div>
				<div id="subheader" class="clear">
					<h3>
                      Good License Header files : <xsl:value-of select="root/results/percentage"/>
					</h3>
				</div>
			</div>
		</div>

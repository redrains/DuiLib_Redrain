<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="/">
		<html>
			<head>
				<title>Duilib控件属性列表</title>
				<style type="text/css">
					td,th{font-family:微软雅黑 幼圆; font-size: 11pt;}
					table.property-grid{width:100%;margin-bottom:30px;border-collapse:collapse;}
					table.property-grid td{border:1px solid black;}
					table.property-grid th{background:#EEEEEE;border:1px solid black;color: #666666;}
				</style>
			</head>
			<body>
				<xsl:call-template name="ListIndex"/>
				<xsl:call-template name="ControlList"/>
			</body>
		</html>
	</xsl:template>

	<xsl:template name="ListIndex">
		<h1><a name="top">Duilib控件属性列表</a></h1>
		<ol>
			<xsl:for-each select="/Controls/*">
			<li>
				<a >
					<xsl:attribute name="href">#<xsl:value-of select="name()"/></xsl:attribute>
					<xsl:value-of select="name()"/>
				</a>
			</li>
			</xsl:for-each>
		</ol>
	</xsl:template>

	<xsl:template name="ControlList">
		<xsl:for-each select="/Controls/*">

			<h2>
				<a>
					<xsl:attribute name="name"><xsl:value-of select="name()"/></xsl:attribute>
					<xsl:value-of select="name()"/>
				</a>
			</h2>

			<table width="100%">
				<tr>
					<td style="width:200px;">
						父类：<a><xsl:attribute name="href">#<xsl:value-of select="@parent"/></xsl:attribute><xsl:value-of select="./@parent"/></a>
					</td>
					<td>
						<span style="float:right;">
							<a href="#top">返回列表</a> . 
							<a>	
								<xsl:if test="preceding-sibling::*">
									<xsl:attribute name="href">#<xsl:value-of select="name(preceding-sibling::*[1])"/></xsl:attribute>
									<xsl:attribute name="title"><xsl:value-of select="name(preceding-sibling::*[1])"/></xsl:attribute>
								</xsl:if>
								上一个
							</a> . 
							<a>
								<xsl:if test="following-sibling::*">
									<xsl:attribute name="href">#<xsl:value-of select="name(following-sibling::*[1])"/></xsl:attribute>
									<xsl:attribute name="title"><xsl:value-of select="name(following-sibling::*[1])"/></xsl:attribute>
								</xsl:if>
								下一个
							</a> 
						</span>
						事件：<xsl:value-of select="./@notifies"/>
					</td>
				</tr>
			</table>

			<table class="property-grid">
				<tr>
					<th width="200px" class="table_titleA">属性名 </th>
					<th width="100px" class="table_titleA"> 默认值</th>
					<th width="60px" class="table_titleA"> 类型</th>
					<th class="table_titleA"> 注释</th>
				</tr>
				<xsl:for-each select="./Attribute">
					<tr>
						<td><xsl:value-of select="./@name"/></td>
						<td><xsl:value-of select="./@default"/></td>
						<td><xsl:value-of select="./@type"/></td>
						<td><xsl:value-of select="./@comment"/></td>
					</tr>
				</xsl:for-each>
			</table>

		</xsl:for-each>
	</xsl:template>

</xsl:stylesheet>

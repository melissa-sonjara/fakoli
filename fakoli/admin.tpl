<!DOCTYPE html>
 <html xmlns="http://www.w3.org/1999/xhtml">
 <head>
  <meta name="description" content="Sonjara CMS Administrative Site">
  <link href='https://fonts.googleapis.com/css?family=Arimo:400,700,400italic,700italic' rel='stylesheet' type='text/css'>
  <title>{var:title}</title>
  {var:styles}
  <link href="/fakoli/css/admin.css" rel="stylesheet"/>
  {var:script}
 </head>
 <body>
  <div id="curtain"></div>
  {var:dialogs}
  <div id="frame">
   <div id="header"><a href="http://www.sonjara.com/" alt="Sonjara, Inc"><h2 style="display: none">Fakoli/CMS Site Administration - a Product of Sonjara, Inc</h2></a></div>
    <div id="nav"><a href="/admin/index">Admin Home</a><span class='sitename'>{config:sitename}</span><div style='float: right'>{help}</div></div>   
    <div id="layout">
     <div id="leftnav">{var:menu}</div>
     <div id="content-admin">
      <div id="container">
       <h2>{var:title}</h2>
       {var:content}
      </div>
     </div>   
    </div>
    <div id="footer">{var:footer}</div>
  </div>
 </body>
</html>
    
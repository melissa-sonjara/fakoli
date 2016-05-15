<!DOCTYPE html>
 <html xmlns="http://www.w3.org/1999/xhtml">
 <head>
  <meta name="description" content="Sonjara CMS Administrative Site">
  <link href='https://fonts.googleapis.com/css?family=Source+Sans+Pro:400,300,600,300italic,400italic,600italic' rel='stylesheet' type='text/css'>
  <link href='https://fonts.googleapis.com/css?family=Open+Sans:400,800' rel='stylesheet' type='text/css'>
  <title>{var:title}</title>
  {var:styles}
  <link href="/fakoli/css/admin.css" rel="stylesheet"/>
  {var:script}
 </head>
 <body>
  <div id="curtain"></div>
  {var:dialogs}
  <div id="frame">
  <div id="leftbar"></div>
   <div id="header"><a href="http://fakoli.org/" alt="A product of Sonjara, Inc"><img src="/fakoli/images/fakoli_logo.svg" id='fakoli_logo'/></a>
   </div>
    <div id="nav"><a href="/admin/index">Admin Home</a><span class='sitename'> WEBSITE: {config:sitename}</span>
    <div style='float: right'>{help}</div></div>   
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
    
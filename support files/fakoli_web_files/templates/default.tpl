<!DOCTYPE html> 
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
 <head>
  <meta name="description" content="{meta_tag_description}" />
  <meta name="keywords" content="{meta_tag_keyword}" />
  <meta property="og:title" content="{page_title}"/>
  <title>{page_title}</title>
  {var:styles}
  <link href="/templates/css/default.css" rel="stylesheet" />
 {var:script}
 <script>
  window.addEvent('domready', function()
  {
  	new FakoliMenu('global');
  });
 </script>
 </head>
 <body>
  <div id="curtain"></div>
  {var:dialogs}
  <div id="frame">
   <div id="header">
    <a href="/index" alt="Fakoli" id="banner"><h2 style="display: none">Fakoli - a PHP Framework &amp; CMS</h2></a>
    {position:login_box}
   </div>
   <div id="layout">
    <div id="nav">{menu:global}</div>    
    <div id="content">
      <h2>{page_title}</h2>
      {description}
    </div>   
   </div>
    <div id="footer">
     {position:footer_content}
    </div>
   </div>
 </body>
</html>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"> 
 <html>
 <head>
  <meta name="description" content="{page_title}: {metadecription}"/>
    <meta name="keywords" content="{page_title}: {keywords}"/>
  <meta http-equiv="content-language" content="en">

    <link href="/default.css" rel="stylesheet"/>
  <title>{page_title}</title>
  

   <script type="text/javascript" src="/js/mootools-core.js"></script>
   <script type="text/javascript" src="/js/mootools-more.js"></script>
   <script type="text/javascript" src="/cms/js/sidebarmenu.js"></script>
   <script type="text/javascript" src="framework/functions.js"></script>
   {var:script}
   {var:styles}
 </head>
 <body>
  <div id="container" align="center">
  <table cellpadding="0" cellspacing="0" id="frame">
   <tr>
   <td colspan="2" id="header"><a href="/">Banner goes here</a></td>
   </tr>
   <tr>
    <td id="leftnav">
    
    {menu:global}
 
    {position:login}
    {position:left}
    </td>    
   <td id="content_cell">
    {position:right}
    <div id="content">
	{position:callout}
    <h2>{page_title}</h2>
    {description} 
    {position:additional_content}   
    </div>
	 </td>
     
    </tr>
    <tr>
   <td id="content_cell">
   <div id="content">
     {position:bottom_boxes}
	 </div>
     </td>
    </tr>
    <tr>
     <td colspan="2">  
     	<div id="footer"><p class="footer">{position:footer}</div>
     </td>
    </tr>
  </table> 
 </body>
</html>
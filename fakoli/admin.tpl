<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"> 
 <html>
 <head>
  <meta name="description" content="Sonjara CMS Administrative Site">
  <link href="/fakoli/css/admin.css" rel="stylesheet"/>
  <link href="/fakoli/css/tree.css" rel="stylesheet"/>
  <link href="/fakoli/css/tables.css" rel="stylesheet"/>
  <link href="/fakoli/css/dialogs.css" rel="stylesheet"/>
  <link href="/fakoli/css/calendar.css" rel="stylesheet"/>
  <link href="/fakoli/css/textarea-resizer.css" rel="stylesheet"/>
  <script type="text/javascript" src="/fakoli/js/functions.js"></script>
  <script type="text/javascript" src="/fakoli/js/mootools-1.2.4-core.js"></script>
  <script type="text/javascript" src="/fakoli/js/mootools-1.2.4-more.js"></script>
  <script type="text/javascript" src="/fakoli/js/ui.js"></script>
  <script type="text/javascript" src="/fakoli/js/iFrameFormRequest.js"></script>
  <script type="text/javascript" src="/fakoli/js/mootools-textarea-resizer.js"></script>
  <script type='text/javascript' src='/fakoli/richtext/sonjara_richtext.js'></script>
  <script type='text/javascript' src='/fakoli/calendar/sonjara_calendar.js'></script>
  <script type="text/javascript">
  window.addEvent('domready', function() 
  {
  	$$('textarea').resizable();
  });  
  </script>
  {var:script}
  <title>{var:title}</title>
 </head>
 <body>
  <div id="curtain"></div>
  {var:dialogs}
  <div id="frame">
   <div id="header"><a href="http://www.sonjara.com/" alt="Sonjara, Inc"><h2 style="display: none">Fakoli/CMS Site Administration - a Product of Sonjara, Inc</h2></a></div>
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
    
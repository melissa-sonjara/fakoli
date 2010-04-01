<?
require_once realpath(dirname(__FILE__)."/../templates/tabs.inc");
 ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"> 
 <html>
 <head>
  <meta name="description" content="Sonjara CMS Administrative Site">
  <link href="/default.css" rel="stylesheet"/>
  <link href="/css/tree.css" rel="stylesheet"/>
  <link href="/cms/css/tables.css" rel="stylesheet"/>
   <script type="text/javascript" src="/framework/functions.js"></script>

   <script type="text/javascript" src="/framework/js/mootools-1.2.4-core.js"></script>
   <script type="text/javascript" src="/framework/js/mootools-1.2.4-more.js"></script>
  <?echo $script?>
  <title><? echo $title?></title>
 </head>
 <body>
  <div id="container" align="center">
  <table cellpadding="0" cellspacing="0" id="frame">
   <tr>
    <td colspan="<?echo $hideMenu?"1":"2"?>" id="header"><h2>CMS Administration</h2></td>
   </tr>
   <tr>
    <?if (!$hideMenu) 
    {
    ?><td id="leftnav" rowspan="2"><?require_once realpath(dirname(__FILE__)."/menu.inc")?></td>
    <?
	} 
	?>
	<td id="topshadow"></td>
	</tr><tr>
    <td id="content-admin"><h2><? echo $title ?></h2>
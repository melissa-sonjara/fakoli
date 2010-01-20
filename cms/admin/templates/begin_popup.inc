<?php
/**
 * begin_help_popup.inc
 * 
 * Description: Template used for the site help pages. 
 * 
 * author: Reshma Joshi for Sonjara, Inc.
 * 
 * date: 06/09 
 *
 **/

$styles .= "<link rel='stylesheet' type='text/css' href='/css/popup.css' />";
$styles .= <<<ENDSTYLES
<style type="text/css">
body {
background-color: white;
}
</style>
ENDSTYLES;

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"> 
 <html>
 <head>
  <meta name="description" content="NCUIH Knowledge Resource Center Dashboard"/>
  <script type="text/javascript" src="/js/mootools-1.2.4-core.js"></script>
  <script type="text/javascript" src="/js/mootools-1.2.4-more.js"></script>
   <script type="text/javascript" src="/framework/functions.js"></script>
   <?echo $script?>
   <?echo $styles?>
  <title>UIH Knowledge Resource Center: <? echo $title?></title>
 </head>
 
<body>
<br/>
<h3><?echo $title ?></h3>

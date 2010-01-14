<?php

require_once "../../include/config.inc";

require_once "../datamodel/publication.inc";
require_once "../../include/permissions.inc";

$menu_item = "Publications";
$title = "Manage Publications";

//obtain the required page number from the $_GET array. Note that if it is not present it will default to 1.
if (isset($_GET['pageno'])) {
   $pageno = $_GET['pageno'];
} else {
   $pageno = 1;
} 

$numrows = count( query(Publication) ); 

$rows_per_page = 20;
$lastpage      = ceil($numrows/$rows_per_page);    //last page number

//see the value of $pageno is an integer between 1 and $lastpage.

$pageno = (int)$pageno;
if ($pageno > $lastpage) {
   $pageno = $lastpage;
} 
if ($pageno < 1) {
   $pageno = 1;
} 

$offset = ($pageno - 1) * $rows_per_page;

$publications = query(Publication, "ORDER BY title LIMIT $offset, $rows_per_page");

require_once admin_begin_page;
?>
<table class="list" width="100%">
 <tr>
  <th>Title</th>
  <th>Publication Year</th>
 </tr>
<?
foreach($publications as $publication)
{
?>
 <tr>
  <td><b><a href="publication_form.php?publication_id=<?echo $publication->publication_id ?>"><?echo $publication->title ?></a></b></td>
  <td><?echo $publication->publication_year ?></td>
 </tr>
<?
}
?>
</table>
<br/>
<br/>
<a href="publication_form.php" class="button"> Add a New Publication </a>


<table style="width: 100%"><tr><td align="right">
<?

if ($pageno == 1) {
	echo " First   Previous ";
} else {
?>
	<a href="<?echo $_SERVER['PHP_SELF'] ?>?pageno=1">First</a> 
<?
   $prevpage = $pageno-1;
?>   
   <a href="<?echo $_SERVER['PHP_SELF'] ?>?pageno=<? echo $prevpage ?>">Previous</a> 
<?
} // if

echo " ( Page $pageno of $lastpage ) ";

if ($pageno == $lastpage) {
	
	echo " Next   Last ";  
} else {
   $nextpage = $pageno+1;
   ?>
   <a href="<?echo $_SERVER['PHP_SELF'] ?>?pageno=<? echo $nextpage ?>">Next</a> 
   <a href="<?echo $_SERVER['PHP_SELF'] ?>?pageno=<? echo $lastpage ?>">Last</a> 
   <?
} 

?>
</td></tr></table>
<br/>
<br/>
<?
require_once admin_end_page;
?>
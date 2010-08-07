<?

require_once "../../include/config.inc";
require_once "../datamodel/topic.inc";
require_once "../datamodel/region.inc";
require_once "../../include/permissions.inc";
require_once "../datamodel/document.inc";
require_once "../datamodel/focus.inc";

require_once "../datamodel/best_practice.inc";
require_once "../datamodel/journal.inc";
require_once "../datamodel/organization.inc";
require_once "../datamodel/sample.inc";
require_once "../datamodel/setting.inc";
require_once "../datamodel/author.inc";
require_once "../framework/join.inc";
require_once "../datamodel/grp.inc";
$title = "Search Results";
$menu_item = "Publications";


$topic_id = checkNumeric($_GET["topic_id"]);
$grp_id = checkNumeric($_GET["grp_id"]);
$region_id = checkNumeric($_GET["region_id"]);
$sample_id = checkNumeric($_GET["sample_id"]);
$setting_id = checkNumeric($_GET["setting_id"]);
$journal_id = checkNumeric($_GET["journal_id"]);
$publication_year = trim($_GET["publication_year"]);
$searchText = trim($_GET["searchText"]);
$author = trim($_GET["author"]);

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

//$publications = query(Publication, "ORDER BY title LIMIT $offset, $rows_per_page");

$query = "WHERE public=1";

$join = new InnerJoin();
$join->add(Publication);

if ($topic_id)
{
	$join->add(PublicationTopicXref);
	$query .= " AND topic_id=$topic_id";	
}
else if ($grp_id)
{
	$grp = new Grp($grp_id);
	$topics = $grp->Topics();
	if (count($topics) > 0)
	{
		$join->add(PublicationTopicXref);
		$query .= " AND topic_id IN (".formatItems($join, "{topic_id}", ", ").")";
	}
}
if ($region_id)
{
	$join->add(PublicationRegionXref);
	$query .= " AND region_id=$region_id";
}

if ($sample_id)
{
	$join->add(PublicationSampleXref);
	$query .= " AND sample_id=$sample_id";
}

if ($setting_id)
{
	$join->add(PublicationSettingXref);
	$query .= " AND setting_id=$setting_id";	
}

if ($journal_id)
{
	
	$query .= " AND journal_id=$journal_id";	
}

if ($publication_year)
{
	$query .= " AND publication_year=". mysql_escape_string($publication_year);
}

if ($searchText)
{
	$searchText = mysql_escape_string($searchText);
	//notworking check here//
	$query .= " AND (title like '%$searchText%' OR keywords like '%$searchText%' OR short_abstract like '%$searchText%' OR abstract like '%$searchText%')";
}

if ($author)
{
	$author = mysql_escape_string($author);
	
	$join->add(Author);
	$join->xref(PublicationAuthorXref);
	
	$query .= " AND author_name like '%$author%'";
}


$query .= " ORDER BY title LIMIT $offset, $rows_per_page ";

//$query .= " ORDER BY title ";
$results = $join->query($query);

$publications = removeDuplicates(extractJoinResults(Publication, $results));





if (count($publications) > 0)
{
	
	
	
?>


<table class="list" width="100%">
 <tr>
  <th>Title/year</th>
  <th>Abstract</th>
 </tr>
<?php
	foreach($publications as $publication)
	{
	?>
<tr>


<td valign="top"><b><a href="publication_preview.php?publication_id=<?echo $publication->publication_id ?>">
<?echo $publication->title ?><?echo $publication->publication_year ?></a></b></td>

<td><?echo $publication->short_abstract ?>

</tr>
<?php 
	}
?>

</table>
<?
}
else
{
?>
<p><i>No matching publications.</i></p>
<?
}
?>
<br/>
<table style="width: 100%"><tr><td align="right">
<?

//$pattern = '/&pageno=(\d+)/i';
// $replacement = '';
 //$url2= preg_replace($pattern, $replacement, $_SERVER['REQUEST_URI']);

if ($pageno == 1) {
	echo " First   Previous ";
} else {
?>
	<a href="<? echo $_SERVER['PHP_SELF'] ?>?<?echo $_SERVER["QUERY_STRING"] ?>&pageno=1">First</a> 
<?
   $prevpage = $pageno-1;
?>   
   <a href="<? echo $_SERVER['PHP_SELF'] ?>?<?echo $_SERVER["QUERY_STRING"] ?>&pageno=<? echo $prevpage ?>">Previous</a> 
<?
} // if

echo " ( Page $pageno of $lastpage ) ";

if ($pageno == $lastpage) {
	
	echo " Next   Last ";  
} else {
   $nextpage = $pageno+1;
  
   ?>
   <a href="<? echo $_SERVER['PHP_SELF'] ?>?<?echo $_SERVER["QUERY_STRING"] ?>&pageno=<? echo $nextpage ?>">Next</a> 
   <a href="<? echo $_SERVER['PHP_SELF'] ?>?<?echo $_SERVER["QUERY_STRING"] ?>&pageno=<? echo $lastpage ?>">Last</a> 
   <?
} 

?>
</td></tr></table>
<br/>
<br/>



<?


?>

<?php
require_once "../../include/config.inc";
require_once "../datamodel/news_item.inc";
require_once "../framework/search_form.inc";


require_once "../../include/permissions.inc";

$newsitem = new NewsItem();
//$filter = new ExclusionFilter();
//$filter->add("locked");

//$item->filter = $filter;

$search = new SearchParameters($newsitem);
$search->fromGET();

$title = "Results for News Item Search";

$newsitems = query(NewsItem, $search->generateConstraint());


?>
<!-- <pre><?print_r($search) ?></pre>
<? echo $search->generateConstraint() ?><br/> -->
<table class="list" width="100%">
 <tr>
  <th>Title</th>
  <th>Message</th>
 </tr> 

<?
foreach ($newsitems as $newsitem)
{
?>
 <tr>
  <td><a href="item_form.php?LOC_key=<?echo $newsitem->news_item_id ?>"><?echo $newsitem->title ?></a></td>
  <td><?echo $newsitem->message ?></td>
 </tr>
<?
}

?>
</table>
<?

?>
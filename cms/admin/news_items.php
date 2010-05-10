<?php
/* =======================================================================
 * NCUIH Online Toolkit
 * Developed by Sonjara, Inc. for NCUIH
 * For further information please contact Andy Green at andy@sonjara.com
 * =======================================================================
 */

require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../datamodel/news_item.inc";

$title = "Articles";
$menu_item = "Articles";

$newsItems = query(NewsItem, "ORDER BY last_modified DESC");

require_once admin_begin_page;

if (count($newsItems) > 0)
{
?>
<table class="list" width="100%">
 <tr>
  <th>Title</th>
  <th>Last Modified</th>
 </tr>
<?
	foreach($newsItems as $item)
	{
?>
 <tr>
  <td><b><a title="<?echo $item->title ?>" href="news_item_form.php?news_item_id=<?echo $item->news_item_id ?>"><?echo ellipsis($item->title, 80) ?></a></b></td>
  <td><?echo formatDate($item->last_modified) ?></td>
 </tr>
<?
	}
?></table>
<?
}
else
{
	echo "<i>No articles have been created yet.</i><br>";
}
?>
<br>
<input type="button" class="button" value="  Add an Article  " onclick="go('news_item_form.php');">
<?
require_once admin_end_page;
?>
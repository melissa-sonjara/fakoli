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

$title = "News Items";
$menu_item = "News Items";

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
  <td><a title="<?echo $item->title ?>" href="news_item_form.php?news_item_id=<?echo $item->news_item_id ?>"><b><?echo ellipsis($item->title, 80) ?></b></a></td>
  <td><?echo formatDate($item->last_modified) ?></td>
 </tr>
<?
	}
?></table>
<?
}
else
{
	echo "<i>No News Items defined</i><br>";
}
?>
<br>
<input type="button" class="button" value="  Add a News Item  " onclick="go('news_item_form.php');">
<?
require_once admin_end_page;
?>
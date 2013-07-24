<?php
Fakoli::using("bookmark");
$page->page_title = "My Bookmarks";

$bookmarks = GroupedQuery::create(Bookmark, "WHERE user_id=:u ORDER BY category, title", "category")
						 ->bind(":u", $user->get($user->getPrimaryKey()))
						 ->execute();

foreach($bookmarks as $category => $links)
{
	if (!$category) $category = "Unclassified";

	echo "<h3>$category</h3>";
	echo "<ul>";

	foreach($links as $link)
	{
		echo $link->format("<li><a href='{url}'>{title}</a>&nbsp;<img src='/fakoli/images/icon_edit.png' alt='Edit' style='cursor: pointer' onclick='BookmarkManager.editBookmark({bookmark_id}); return false'/>");
	}
	
	echo "</ul>";
}
	
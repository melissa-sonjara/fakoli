<?
Fakoli::using("bookmark");

if(!$user) return;

if (Bookmark::findBookmark())
{
?>
You have bookmarked this page
<?
}
else
{
?>
<a href='#' class='bookmark_link' onclick='BookmarkManager.bookmarkPage(); return false'>Bookmark this Page</a>
<?
}
?>
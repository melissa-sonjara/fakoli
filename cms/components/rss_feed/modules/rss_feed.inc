<?php
/**
 * Display links to various twitter feeds.
 */
Fakoli::using("rss_feed_articles");
?>
<div id="twitter_feed_container">
<?php 
$rssFeeds = Query::create(RSSFeedArticles, "WHERE publish_status=1")
	->execute();

if(count($rssFeeds))
{
	echo "<div>\n";
	foreach($rssFeeds as $rssFeed)
	{
		echo "<a href='{$rssFeed->rss_URL}'>{$rssFeed->rss_title}</a></br>";  
		echo "{$rssFeed->date_added} </br>"; //Date - URL
		echo "<p>";
	}
	echo "</div>\n";
}

?>

</div>
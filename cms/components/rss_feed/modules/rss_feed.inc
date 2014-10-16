<?php
/**
 * Display links to various twitter feeds.
 */
Fakoli::using("rss_feed");
?>
<div id="rss_feed_container">
<?php 
$rssFeeds = RSSFeedArticles::getPublishedArticles(10);

if(count($rssFeeds))
{
	foreach($rssFeeds as $rssFeed)
	{
		echo $rssFeed->format("<div><a href='{rss_article_URL}'>{rss_title}</a></br>{article_date:F j, Y} - {RSSFeed.rss_title}</div>");  
	}
}

?>
</div>
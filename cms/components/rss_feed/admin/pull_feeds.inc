<?php
Fakoli::using("rss_feed");
Fakoli::usingFeature("data_view");

$menu_item = "RSS Feeds";

$rssFeeds = Query::create(RSSFeed, "ORDER BY rss_feed_id")->execute();
echo "The following feed content have been updated:</br>";

foreach($rssFeeds as $feed)
{
	echo $feed_id = $feed->rss_title."</br>";
	
	$rssurl = simplexml_load_file($feed->rss_url,"SimpleXMLElement", LIBXML_NOCDATA);
	
	foreach ($rssurl->channel->item as $item) {		
		
		var_dump($item);
		
		$rss = new RSSFeedArticles();
		
		$rss->rss_feed_id			= $feed->rss_feed_id;
		$rss->rss_title		 		= $item->title;
		$rss->article_date		 	= date("Y-m-d H:i:s", strtotime($item->pubDate));
		$rss->rss_article_URL		= $item->link;
		
		$rss->publish_status = $feed->trusted_feed;
		
		$rss->save(); 
	}
}	
?>
<br/>
<a class='button' href='rss_feed'>&laquo; Back to RSS Feed List</a>
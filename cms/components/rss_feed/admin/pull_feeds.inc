<?php
Fakoli::using("rss_feed");
Fakoli::usingFeature("data_view");

$rssFeeds = Query::create(RSSFeed, "ORDER BY rss_feed_id")->execute();
echo "The following feed content have been updated:</br>";

foreach($rssFeeds as $feed){
	echo $feed_id = $feed->rss_title."</br>";
	
	$rssurl = simplexml_load_file($feed->rss_url,"SimpleXMLElement", LIBXML_NOCDATA);
	//$rss_feed_id = $feed->rss_feed_id;
	
	foreach ($rssurl->channel->item as $item) {		
		
		$rss = new RSSFeedArticles();
		
		$rss->rss_feed_id			= $feed->rss_feed_id;;
		$rss->rss_title		 		= $item->title;
		$rss->date_added		 	= $item->pubDate;
		$rss->rss_article_URL		= $item->link;
		
		$rss->save(); 
	}
}	
?>
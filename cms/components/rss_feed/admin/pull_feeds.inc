<?php
Fakoli::using("rss_feed_articles");
Fakoli::using("rss_feed");
Fakoli::usingFeature("data_view");

$rssFeeds = Query::create(RSSFeed, "ORDER BY rss_feed_id")->execute();
echo "The following feed content have been updated:</br>";

foreach($rssFeeds as $feed){
	
	echo formatItems($rssFeeds, "{rss_title}", "</br>");
	
	$rssurl = simplexml_load_file($feed->rss_url,"SimpleXMLElement", LIBXML_NOCDATA);
	
	foreach ($rssurl->channel->item as $item) {		
		$rss = new RSSFeedArticles();
		$rss->rss_title		 		= $item->title;
		$rss->rss_article_author 	= $item->children('http://purl.org/dc/elements/1.1/');
		$rss->rss_article_newsStory = $item->description;
		
		$rss->save(); 
	}
}	
?>
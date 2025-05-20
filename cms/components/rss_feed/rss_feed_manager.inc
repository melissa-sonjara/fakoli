<?php
/**\addtogroup rss_feed
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the rss_feed component.
 */
Fakoli::using("settings","rss_feed");
Fakoli::usingFeature("data_view");

class RssFeedManager
{
	function __construct()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	static function upgradeComponent($version)
	{
		$mgr = new RssFeedUpgradeManager();
		$mgr->upgrade($version);
	}
	
	function pullFeed(){
		$rssFeeds = Query::create(RSSFeed, "ORDER BY rss_feed_id")->execute();
		echo "The following feed content have been updated:</br>";
		
		foreach($rssFeeds as $feed)
		{
			echo $feed_id = $feed->rss_title."</br>";
			
			$rssurl = simplexml_load_file($feed->rss_url,"SimpleXMLElement", LIBXML_NOCDATA);
			
			foreach ($rssurl->channel->item as $item) 
			{			
				try
				{
					$rss = $feed->findArticleByURL($item->link);
				}
				catch(DataNotFoundException $e)
				{
					$rss = new RSSFeedArticles();
				}
				
				$rss->rss_feed_id			= $feed->rss_feed_id;
				$rss->rss_title		 		= $item->title;
				$rss->article_date		 	= date("Y-m-d H:i:s", strtotime($item->pubDate));
				$rss->rss_article_URL		= $item->link;
				$rss->rss_article_newsStory = $item->description;
				$rss->rss_article_content	= $item->children('content',true)->encoded;
				$rss->publish_status 		= $feed->rss_trusted;
				
				$rss->save(); 
			}
		}		
	}
	
	static function registerScheduledRSSWorkers(){
		ScheduledTaskManager::registerWorker("RSS", "RSS Updates", array(RssFeedManager, pullFeed));
	}
}

/**@}*/?>

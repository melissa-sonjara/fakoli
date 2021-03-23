<?php
/**\addtogroup rss_feed
 * @{
 */


class RSSFeedArticles extends DataItem
{
	var $table = "rss_feed_articles";
	var $primary_key = "rss_article_id";
	
	var $fields = array(
		"rss_article_id"		=>	Number,
		"rss_title"				=>	Text,
		"rss_feed_id"			=>	Number,
		"rss_article_author"	=>	String,
		"rss_article_newsStory"	=>	Text,
		"rss_article_content"	=>	Text,
		"rss_article_URL"		=>	String,
		"rss_thumbnail"			=>	String,
		"article_date"			=>	DateTime,
		"publish_status"		=>	Boolean,
		"date_added"			=>	Timestamp
		);
	
	var $relations = array("RSSFeed"	=>	RSSFeed);

	function RSSFeed()
	{
		return $this->getRelated(RSSFeed);
	}
	
	static function getPublishedArticles($limit = 10)
	{
		return Query::create(RSSFeedArticles, "WHERE publish_status=1 ORDER BY article_date DESC LIMIT $limit")->execute();
	}		
}
/**@}*/?>
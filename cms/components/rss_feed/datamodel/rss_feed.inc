<?php
/**\addtogroup rss_feed
 * @{
 */


class RSSFeed extends DataItem
{
	var $table = "rss_feed";
	var $primary_key = "rss_feed_id";
	
	var $fields = array(
		"rss_feed_id"	=>	Number,
		"rss_title"		=>	String,
		"rss_url"		=>	String,
		"rss_trusted"	=>	Boolean,
		"last_updated"	=>	Timestamp,
		"feed_status"	=>	Boolean
		);
	
	var $relations = array(
			"Articles"	=> RSSFeedArticles
			);
	
	var $fieldAliases = array(
		"feed_status"	=>	"Active",
		"rss_trusted"	=>	"Trusted Feed"
			);
	
	function Articles($constraint = "")
	{
		return $this->getRelatedList(RSSFeedArticles, "", $constraint);
	}

	static function getrssFeedsForUser($user_id)
	{
		$feeds = Query::create(rssFeed, "WHERE rss_feed_id in (SELECT rss_feed_id FROM rss_feed_articles where user_id=:u) ORDER BY sort_order")
		 			  ->bind(":u", $user_id)
		 			  ->execute();
		
		if (count($feeds) == 0)
		{
			$feeds = Query::create(rssFeed, "WHERE feed_status=1 ORDER BY sort_order")->execute();
		}
		
		return $feeds;
	}

	function findArticleByURL($url)
	{
		return Query::create(RSSFeedArticles, "WHERE rss_feed_id=:f AND rss_article_URL=:u")
					->bind(":f", $this->rss_feed_id, ":u", $url)
					->executeSingle();}
		
}
/**@}*/?>
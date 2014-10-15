<?php
class RSSFeedArticles extends DataItem
{
	var $table = "rss_feed_articles";
	var $primary_key = "rss_article_id";
	
	var $fields = array(
		"rss_article_id"		=>	Number,
		"rss_title"				=>	Text,
		"rss_feed_id"			=>	Number,
		"rss_article_author"	=>	String,
		"rss_article_URL"		=>	Text,
		"publish_status"		=>	Boolean,
		"date_added"			=>	Timestamp
		);
	
	var $relations = array(
			
			);

}
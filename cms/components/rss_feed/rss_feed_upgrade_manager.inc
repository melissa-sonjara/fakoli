<?php
/**\addtogroup rss_feed
 * @{
 */


/**
 * Manage schema updates for the rss_feed component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Fiifi Baidoo
 */


Fakoli::using("rss_feed", "component");

class RssFeedUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Create rss_feed and rss_feed_article tables",
		"1.1"	=>	"sql:Added missing fields, increased varchar field lengths",
		"1.2"	=>	"sql:Added missing publish_status field to rss_feed_article",
		"1.3"	=>	"sql:Added index on rss_feed_article rss_article_URL field",
		"1.4"	=>	"sql:Added column on rss_feed_article rss_article_content field",
		"1.5"	=>	"sql:Added column on rss_feed_article rss_thumbnail field to save images from the feeds"
	);

	function __construct()
	{
		$this->component_name = "rss_feed";

		// sets the sqlFile name according to default
		parent::__construct();
	}
}

/**@}*/?>

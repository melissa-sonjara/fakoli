<?php
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
		"1.0"	=>	"sql:Create rss_feed and rss_feed_article tables"
	);

	function RssFeedUpgradeManager()
	{
		$this->component_name = "rss_feed";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
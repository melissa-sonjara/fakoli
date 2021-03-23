<?php
/**\defgroup rss_feed Rss Feed
 * @{
 */


class RssFeedManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "rss_feed";
		$component->description = "Component to consume and display RSS feeds";
		$component->author = "Fiifi Baidoo";
		$component->version = "1.1";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array(
			"Collaboration"	=> array
			(

				"RSS Feeds"	=>	array("page"	=>	"/admin/rss_feed",
									  "role"	=>	"admin,editor",
									  "weight"	=>	10,
									  "icon"	=>	"rss")
			)
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"	=>	array(RssFeedManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(RssFeedManager, setDefaults),
					"RegisterScheduledTaskWorkers"	=> array(RssFeedManager, registerScheduledRSSWorkers)
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>
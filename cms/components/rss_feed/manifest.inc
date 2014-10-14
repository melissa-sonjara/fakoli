<?
class RssFeedManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "rss_feed";
		$component->description = "Component to consume and display RSS feeds";
		$component->author = "Fiifi Baidoo";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array(
			"Classification"	=> array
			(

				"RSS Feeds"	=>	array("page"	=>	"/admin/rss_feed",
							"role"	=>	"admin,editor",
							"weight"	=>	10)
			)
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"	=>	array(RssFeedManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(RssFeedManager, setDefaults)
					//TODO: Add Event Registrations here
		);
	}
}

?>
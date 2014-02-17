<?
class RatingManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "rating";
		$component->description = "Page rating component";
		$component->author = "Andrew Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array(
				//TODO: Admin Menu Item definitions
		);
	}

	static function getStyles()
	{
		return array(
				"/components/rating/css/page_rating.css"
		);
	}

	static function getScripts()
	{
		return array(
				"/components/rating/js/page_rating.js"
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"	=>	array(RatingManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(RatingManager, setDefaults)
					//TODO: Add Event Registrations here
		);
	}
}

?>
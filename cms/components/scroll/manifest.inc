<?
class ScrollManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "scroll";
		$component->description = "Provides support for long scrolling pages with multiple blocks of content";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 5;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array
		(
			"Site Content"	=> array
			(
				"Scrolls"	=>	array("page"	=>	"/admin/scrolls",
									  "role"	=>	"admin,editor",
								  	  "weight"	=>	3)
			)
		);		
	}	

	static function getStyles()
	{
		return array(
				//TODO: Add CSS File References "/components/scroll/css/..."
		);
	}

	static function getScripts()
	{
		return array(
				//TODO: Add Javascript File References "/components/scroll/js/..."
		);
	}

	static function subscribeToEvents()
	{
		return array("upgradeComponent"			=> array(ScrollManager, upgradeComponent),
					 "ComponentScanComplete"	=> array(ScrollManager, setDefaults),
					 "ResolveIdentifier" 		=> array(ScrollManager, displayScroll),
					 "EnumerateItems"			=> array(ScrollManager, enumerateScrolls),
					 //"RegisterSearchables"			=> array(PageManager, registerSearchables),
					 "RegisterTaxonomyClasses"	=> array(ScrollManager, registerTaxonomyClasses),
				
					//TODO: Add Event Registrations here
		);
	}
}

?>
<?php
/**\defgroup scroll Scroll
 * @{
 */


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
				"Scrolls"	=>	array("page"		=>	"/admin/scrolls",
									  "role"		=>	"admin,editor",
								  	  "weight"	=>	4,
									  "icon"		=>	"/components/scroll/images/scroll.svg")
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
				"/components/scroll/js/scroll_part_manager.js"
		);
	}

	static function subscribeToEvents()
	{
		return array("upgradeComponent"				=> 'ScrollManager::upgradeComponent',
					 "ComponentScanComplete"		=> 'ScrollManager::setDefaults',
					 "ResolveIdentifier" 			=> 'ScrollManager::displayScroll',
					 "EnumerateItems"				=> 'ScrollManager::enumerateScrolls',
					 //"RegisterSearchables"			=> 'PageManager::registerSearchables',
					 "RegisterTaxonomyClasses"		=> 'ScrollManager::registerTaxonomyClasses',
					 "RegisterSerializationHandler" => 'ScrollManager::registerSerializationHandler',
					 "RegisterVersionedContent"		=> 'ScrollManager::registerVersionedContent'
				
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>
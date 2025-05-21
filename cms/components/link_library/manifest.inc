<?php
/**\defgroup link_library Link Library
 * @{
 */


class LinkLibraryManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "link_library";
		$component->description = "A simple collection of links";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array
		(
			"Site Content"	=> array
			(

				"Links"	=>	array("page"		=>	"/admin/link_libraries",
								  "role"		=>	"admin,editor",
								  "weight"	=>	10,
								  "icon"		=>	"link")
			)
		);
	}

	static function getStyles()
	{
		return array(
				"/components/link_library/css/link_library.css"
		);
	}

	static function getScripts()
	{
		return array(
				"/components/link_library/js/link_library.js"
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"EnumerateItems"	 => 'LinkLibraryManager::enumerateItems',
					"ResolveIdentifier"	 =>	'LinkLibraryManager::displayLibrary',
					"RegisterSearchables"	=>	'LinkLibraryManager::registerSearchables',
					"upgradeComponent"	=>	'LinkLibraryManager::upgradeComponent',
					"ComponentScanComplete"	=>	'LinkLibraryManager::setDefaults',
					"RegisterSerializationHandler"	=>	'LinkLibraryManager::registerSerializationHandler',
					"RegisterTaxonomyClasses"	=>	'LinkLibraryManager::registerTaxonomyClasses',
					"DeleteUser"			=>	'LinkLibraryManager::deleteUser',
		);
	}
}

/**@}*/?>
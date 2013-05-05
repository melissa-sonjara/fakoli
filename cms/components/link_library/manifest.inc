<?
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

				"Links"	=>	array("page"	=>	"/admin/link_libraries",
								  "role"	=>	"admin,editor",
								  "weight"	=>	4)
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
					"EnumerateItems"	 => array(LinkLibraryManager, enumerateItems),
					"ResolveIdentifier"	 =>	array(LinkLibraryManager, displayLibrary),
					"RegisterSearchables"	=>	array(LinkLibraryManager, registerSearchables),
					"upgradeComponent"	=>	array(LinkLibraryManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(LinkLibraryManager, setDefaults),
					"RegisterSerializationHandler"	=>	array(LinkLibraryManager, registerSerializationHandler),
					"RegisterTaxonomyClasses"	=>	array(LinkLibraryManager, registerTaxonomyClasses),
					"DeleteUser"			=>	array(LinkLibraryManager, deleteUser),
		);
	}
}

?>
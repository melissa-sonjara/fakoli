<?
class DeveloperToolsManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "developer_tools";
		$component->description = "Provides tools and reports useful for understanding a Fakoli application's configuration and structure.";
		$component->author = "Andrew Green";
		$component->version = "1.0";
		$component->priority = 20;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array(			
			"Developer Tools"	=>	array
			(
				"Content Inventory"	=>	array("page"	=>	"/admin/content_inventory",
										  	  "role"	=>	"admin",
										  	  "weight"	=>	1,
											  "icon"	=>	"list-alt"),
					
				"Section Map"		=>	array("page"	=>	"/admin/section_map",
											  "role"	=>	"admin",
											  "weight"	=>	2,
											  "icon"	=>	"map"),
					
				"Event Map"			=>	array("page"	=>	"/admin/event_map",
											  "role"	=>	"admin",
											  "weight"	=>	3,
											  "icon"	=>	"map-signs"),
				
				"Permissions Map"	=>	array("page"	=>	"/admin/permissions_map",
											  "role"	=>	"admin",
											  "weight"	=>	4,
											  "icon"	=>	"unlock"),
					
				"Test PHP CLI"		=>	array("page"	=>	"/admin/test_php_cli",
											  "role"	=>	"admin",
											  "weight"	=>	99,
											  "icon"	=>	"terminal")
			)
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(DeveloperToolsManager, setDefaults),
					"RegisterAdminIcons"		=>	array(DeveloperToolsManager, registerIcons)
					//TODO: Add Event Registrations here
		);
	}
}

?>
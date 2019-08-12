<?
class GlossaryManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "glossary";
		$component->description = "Terms and their definitions.";
		$component->author = "Stephen Omwony";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array(
		    "Site Content"	=> array
		    (
		        "Glossary"	=>	array("page"		=>	"/admin/glossaries",
		            "role"		=>	"admin",
		            "weight"	=>	11,
		            "icon"		=>	"")
		    )
		);
	}

	static function getStyles()
	{
		return array(
				//TODO: Add CSS File References "/components/glossary/css/..."
		);
	}

	static function getScripts()
	{
		return array(
				//TODO: Add Javascript File References "/components/glossary/js/..."
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"	=>	array(GlossaryManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(GlossaryManager, setDefaults)
					//TODO: Add Event Registrations here
		);
	}
}

?>
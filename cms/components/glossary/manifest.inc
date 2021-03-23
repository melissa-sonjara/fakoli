<?php
/**\defgroup glossary Glossary
 * @{
 */


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
				"/components/glossary/css/glossary.css"
		);
	}

	static function getScripts()
	{
		return array(
				"/components/glossary/js/glossarizer.js"
		);
	}

	var $allow_sessionless_handlers = array("glossary_list");
	
	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"		=>	array(GlossaryManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(GlossaryManager, setDefaults),
					"StartPage"				=>	array(GlossaryManager, glossarizeContent)
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>
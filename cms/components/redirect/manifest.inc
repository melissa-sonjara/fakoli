<?
class RedirectManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "redirect";
		$component->description = "Provides redirection for resources that change identifier or move to a different section or site";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 30;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array
		(
			"Site Configuration"	=> array
			(

				"Redirects"	=>	array("page"		=>	"/admin/redirects",
									  "role"		=>	"admin",
								 	  "weight"	=>	11,
									  "icon"		=>	"redo-alt")
			)
		);	
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"		=>	array(RedirectManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(RedirectManager, setDefaults),
					"ResourceNotFound"	 	=>	array(RedirectManager, handleRedirects),
					"SectionNotFound"		=>	array(RedirectManager, handleSectionRedirects),
					"OverrideResource"		=>	array(RedirectManager, handleOverrides),
					"RegisterSerializationHandler"	=>	array(RedirectManager, registerSerializationHandler)
		);
	}
}

?>
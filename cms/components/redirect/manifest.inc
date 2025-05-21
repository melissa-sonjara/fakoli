<?php
/**\defgroup redirect Redirect
 * @{
 */


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
					"upgradeComponent"		=>	'RedirectManager::upgradeComponent',
					"ComponentScanComplete"	=>	'RedirectManager::setDefaults',
					"ResourceNotFound"	 	=>	'RedirectManager::handleRedirects',
					"SectionNotFound"		=>	'RedirectManager::handleSectionRedirects',
					"OverrideResource"		=>	'RedirectManager::handleOverrides',
					"RegisterSerializationHandler"	=>	'RedirectManager::registerSerializationHandler'
		);
	}
}

/**@}*/?>
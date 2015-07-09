<?php
/**
 * Manage schema updates for the redirect component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("redirect", "component");

class RedirectUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Added redirect table",
		"1.1"	=>	"sql:Added override field to redirect table"
	);

	function RedirectUpgradeManager()
	{
		$this->component_name = "redirect";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
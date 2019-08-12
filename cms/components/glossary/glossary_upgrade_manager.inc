<?php
/**
 * Manage schema updates for the glossary component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Stephen Omwony
 */


Fakoli::using("glossary", "component");

class GlossaryUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
	    "1.0"	=>	"sql:Initial Installation",
	    "1.1"	=>	"sql:Created glossary table"
	);

	function GlossaryUpgradeManager()
	{
		$this->component_name = "glossary";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
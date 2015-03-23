<?php
/**
 * Manage schema updates for the open_data component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Siobhan Green
 */


Fakoli::using("OpenData", "component");

class OpenDataUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		//"x.x"	=>	"description"
		
			"1.1"	=>	"created the open_data table",
			"1.2"	=>	"added ignore item to the open_data table to ignore item from data dictionary"
	);

	function OpenDataUpgradeManager()
	{
		$this->component_name = "OpenData";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
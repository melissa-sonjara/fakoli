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
		
			"1.1"	=>	"created the open_data table"
	);

	function OpenDataUpgradeManager()
	{
		$this->component_name = "OpenData";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
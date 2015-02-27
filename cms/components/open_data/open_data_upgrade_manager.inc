<?php
/**
 * Manage schema updates for the open_data component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Siobhan Green
 */


Fakoli::using("open_data", "component");

class OpenDataUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		//"x.x"	=>	"description"
	);

	function OpenDataUpgradeManager()
	{
		$this->component_name = "open_data";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
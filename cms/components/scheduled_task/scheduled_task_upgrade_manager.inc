<?php
/**
 * Manage schema updates for the scheduled_task component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("scheduled_task", "component");

class ScheduledTaskUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		//"x.x"	=>	"description"
	);

	function ScheduledTaskUpgradeManager()
	{
		$this->component_name = "scheduled_task";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
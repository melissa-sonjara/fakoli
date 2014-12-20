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
		"1.0"	=>	"sql:Created scheduled_task and scheduled_task_log_entry tables",
		"1.1"	=>	"sql:Removed next_run field from scheduled_task table"
	);

	function ScheduledTaskUpgradeManager()
	{
		$this->component_name = "scheduled_task";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
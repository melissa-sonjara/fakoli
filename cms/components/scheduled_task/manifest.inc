<?php
/**\defgroup scheduled_task Scheduled Task
 * @{
 */


class ScheduledTaskManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "scheduled_task";
		$component->description = "Scheduling for periodic background tasks.";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 25;
		$component->enabled = true;

		return $component;
	}

	static function getStyles()
	{
		return array(
				"/components/scheduled_task/css/scheduled_task.css"
				);
	}
	
	static function getAdminMenu()
	{
		return array(
			"Site Configuration"	=> array
			(

				"Scheduled Tasks"	=>	array("page"	=>	"/admin/scheduled_tasks",
											  "role"	=>	"admin",
								  			  "weight"	=>	13,
											  "icon"		=>	"clock")
			)
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"	=>	'ScheduledTaskManager::upgradeComponent',
					"ComponentScanComplete"	=>	'ScheduledTaskManager::setDefaults',
					 "RegisterScheduledTaskWorkers"	=>	'ScheduledTaskManager::registerScheduledTaskWorkers'
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>
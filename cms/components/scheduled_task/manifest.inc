<?
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

	static function getAdminMenu()
	{
		return array(
				//TODO: Admin Menu Item definitions
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"	=>	array(ScheduledTaskManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(ScheduledTaskManager, setDefaults)
					//TODO: Add Event Registrations here
		);
	}
}

?>
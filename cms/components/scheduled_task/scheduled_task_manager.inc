<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the scheduled_task component.
 */

class ScheduledTaskManager
{
	static $workers = array();
	
	function ScheduledTaskManager()
	{
		ComponentManager::fireEvent("RegisterScheduledTaskWorkers");
	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	static function registerWorker($component, $task_name, $handler)
	{
		ScheduledTaskManager::$workers[$component.":".$task_name] = $handler;
		
		try
		{
			$task = ScheduledTask::getInstance($component, $task_name);
		}
		catch(DataNotFoundException $e)
		{
			$task = new ScheduledTask();
			$task->component = $component;
			$task->task_name = $task_name;
			$task->active = false;
			$task->save();
		}
	}

	function getTasks()
	{
		return Query::create(ScheduledTask, "ORDER BY component, task_name")->execute();
	}
	
	function executeTask($component, $task_name)
	{
		$task = ScheduledTask::getInstance($component, $task_name);
		
		if (!$task)
		{
			// Report an error
		}

		$timestamp = now();
		
		$worker = ScheduledTaskManager::$workers[$component.':'.$task_name];
		ob_start();
		$result = call_user_func($worker);
		$output = ob_get_contents();
		ob_end_clean();
		
		$log = new ScheduledTaskLogEntry();
		$log->task_id = $task->task_id;
		$log->log_date = $timestamp;
		$log->log = $output;
		$log->status = $result;
		$log->save();
		
		//$task->recordRun();
	}
	
	function runTasks()
	{
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new ScheduledTaskUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>
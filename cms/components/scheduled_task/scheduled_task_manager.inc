<?php
Fakoli::using("email");

/**
 * Manages scheduled task registration and execution. Scheduled tasks are tasks
 * that can be run periodically without user initiation, triggered by a cron job
 * or Windows scheduled task. Components can register tasks by subscribing to
 * the RegisterScheduledTaskWorkers event. Registered tasks can then be 
 * configured for scheduled execution via the admin screens.
 */
class ScheduledTaskManager
{
	static $workers = array();
	
	/**
	 * Creates a ScheduledTaskManager.
	 */
	function __construct()
	{
		ComponentManager::fireEvent("RegisterScheduledTaskWorkers");
	}

	/**
	 * Registers a simple task that sends an email when triggered. This can be used
	 * to validate the configuration of the cron job / scheduled task environment.
	 */
	static function registerScheduledTaskWorkers()
	{
		ScheduledTaskManager::registerWorker("scheduled_task", "Validate Configuration", array(ScheduledTaskManager, testTask));
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue('scheduled_task', 'test_email', '', 'String', 'Comma-separated list of recipients for the test task');
	}

	/**
	 * Registers a Scheduled Task worker method. This method should be called from within 
	 * the component's RegisterScheduledTaskWorkers event handler. Components are free to 
	 * register as many scheduled task workers as they need. 
	 * @param string $component the name of the component that owns the worker
	 * @param string $task_name the display name of the task
	 * @param callback $handler the handler method to call to execute the task
	 * @param boolean $hourly true if this task should always be executed every hour, false if
	 * the task should be explicitly scheduled by the user
	 */
	static function registerWorker($component, $task_name, $handler, $hourly = false)
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
			$task->run_every_hour = $hourly;
			$task->save();
		}
	}

	static function getTabs($key)
	{
		$tabs = array("Task Schedule"	=>	"/admin/scheduled_task_form",
					  "Execution Logs"	=>	"/admin/scheduled_task_logs");
	
		$qs = ($key) ? "task_id=$key" : "";
		return new TabBar("tabs", $tabs, $qs);
	}
	
	/**
	 * Retrieves all the registered scheduled tasks
	 */
	function getTasks()
	{
		return Query::create(ScheduledTask, "ORDER BY component, task_name")->execute();
	}
	
	/**
	 * Executes the specified scheduled task
	 * @param string $component the name of the component that owns the task
	 * @param string $task_name the name of the task to be executed
	 */
	function executeTask($component, $task_name)
	{
		trace("-- Executing Task '{$component}: $task_name'", 3);
		
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
	}
	
	/**
	 * Runs all the tasks that are scheduled to be run for the current period (day and hour).
	 */
	function runTasks()
	{
		global $config;
		
		Fakoli::scheduledTaskTraceLevel();
		
		$currentPeriod = TaskScheduleHelper::calculatePeriod();
		trace("** Running tasks for period $currentPeriod", 3);
		
		$tasks = $this->getTasks();
		$count = 0;
		
		foreach($tasks as $task)
		{
			if (!$task->active) continue;
			if ($task->run_every_hour || strpos($task->schedule, $currentPeriod) !== false)
			{
				$this->executeTask($task->component, $task->task_name);
				++$count;
			}
		}
		
		trace("$count ".pluralize("task", $count)." executed", 3);

		Fakoli::coreTraceLevel();
	}
	
	/**
	 * Provides the implementation of the test task, that can be used to validate the 
	 * scheduled task server configuration (cron job, etc)
	 */
	static function testTask()
	{
		global $config;
		
		trace("Running Validation Configuration task", 3);
		
		$email = Settings::getValue('scheduled_task', 'test_email');
		if (!$email) 
		{
			$email = Settings::getValue("settings", "system_notification_email");
		}
		
		$site_email_from =  Settings::getValue('email', 'email_from');
		$site_email_name =  Settings::getValue('email', 'email_name');
		
		if (!$email) return;
		
		trace("Sending confirmation email to {$email}", 3);
		
		$mail = new EmailHandler($email, "Scheduled Task Notification from {$config['site_name']}", 
								"This message confirms that the Validate Configuration schedule task worker was triggered at ".now(), 
								$site_email_from, 
								$site_email_name);
		$mail->send();
 			
		echo "Confirmation email sent";
		
		return "Success";
	}
		
	static function upgradeComponent($version)
	{
		$mgr = new ScheduledTaskUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>
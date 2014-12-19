<?php
class ScheduledTask extends DataItem
{
	var $table = "scheduled_task";
	var $primary_key = "task_id";
	
	var $fields = array("task_id"		=>	Number,
						"component"		=>	String,
						"task_name"		=>	String,
						"schedule"		=>	TaskSchedule,
						"active"		=>	Boolean,
						"next_run"		=>	DateTime);
	
	var $relations = array("LogEntries"	=>	ScheduledTaskLogEntry,
						   "LastRun"	=>	ScheduledTaskLogEntry);
	
	function LogEntries($constraint = "")
	{
		return $this->getRelatedList(ScheduledTaskLogEntry, "", $constraint);
	}
	
	static function getInstance($component, $task_name)
	{
		return Query::create(ScheduledTask, "WHERE component=:c AND task_name=:t")
				    ->bind(":c", $component, ":t", $task_name)
				    ->executeSingle();
	}
	
	function LastRun()
	{
		if ($this->_lastRun) return $this->_lastRun;
		$lastRun = $this->LogEntries("ORDER BY log_date DESC limit 1");
		$this->_lastRun = $lastRun;
		return $lastRun;
	}
}	


class ScheduledTaskLogEntry extends DataItem
{
	var $table = "scheduled_task_log_entry";
	var $primary_key = "log_entry_id";
	
	
	var $fields = array("log_entry_id"	=>	Number,
						"task_id"		=>	Number,
						"log_date"		=>	DateTime,
						"log"			=>	Text,
						"status"		=>	String);
	
	var $relations = array("Task"	=>	ScheduledTask);
	
	function Task()
	{
		return $this->getRelated(ScheduledTask);
	}
}

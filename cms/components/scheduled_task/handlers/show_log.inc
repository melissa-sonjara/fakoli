<?php
Fakoli::using("scheduled_task");

$task_id = checkNumeric($_GET["task_id"]);
$log_entry_id = checkNumeric($_GET["log_entry_id"]);

if (!$task_id || !$log_entry_id)
{
	Fakoli::end("Missing Parameter");
}

$log = new ScheduledTaskLogEntry($log_entry_id);
if ($log->task_id != $task_id)
{
	Fakoli::end("Parameter mismatch");
}

echo $log->log;

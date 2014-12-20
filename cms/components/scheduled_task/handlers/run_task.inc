<?php
Fakoli::using("scheduled_task");

if (!checkRole("admin"))
{
	throw new FakoliException("Permission Denied");
}

$task_id = checkNumeric($_GET["task_id"]);
if (!$task_id)
{
	throw new FakoliException("No task specified");
}

$task = new ScheduledTask($task_id);
$task->run();

redirect("scheduled_task_logs?task_id={$task_id}");

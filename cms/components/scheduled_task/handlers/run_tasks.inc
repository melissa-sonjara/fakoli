<?php
Fakoli::using("scheduled_task");

if (!checkRole("admin") && $_SERVER["REMOTE_ADDR"] != $_SERVER["SERVER_ADDR"] && $_SERVER["REMOTE_ADDR"] != "127.0.0.1")
{
	throw new FakoliException("Scheduled tasks cannot be run from {$_SERVER["REMOTE_ADDR"]}");
}

$mgr = new ScheduledTaskManager();
$mgr->runTasks();
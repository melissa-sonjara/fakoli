<?php
Fakoli::using("scheduled_tasks");
Fakoli::usingFeature("tab_bar", "auto_form");

$task_id = checkNumeric($_GET["task_id"]);

if (!$task_id) redirect("scheduled_tasks");

$task = new ScheduledTask($task_id);
$form = new AutoForm($task);
$form->readonly("component", "task_name", "next_run");

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("scheduled_tasks");
	}
}

$tabs = ScheduledTaskManager::getTabs();

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div class='tab_border'>
<?
$form->drawForm();
?>
</div>
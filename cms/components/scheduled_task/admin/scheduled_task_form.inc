<?php
Fakoli::using("scheduled_task");
Fakoli::usingFeature("tab_bar", "auto_form");

$menu_item = "Scheduled Tasks";

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

$tabs = ScheduledTaskManager::getTabs($task_id);

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div class='tab_border'>
<?
$form->drawForm();
?>
</div>
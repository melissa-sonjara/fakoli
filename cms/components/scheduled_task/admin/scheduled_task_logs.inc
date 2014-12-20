<?php
Fakoli::using("scheduled_task");
Fakoli::using("data_view");

$menu_item = "Scheduled Tasks";

$task_id = checkNumeric($_GET["task_id"]);
if (!$task_id)
{
	redirect("scheduled_tasks");
}

$task = new ScheduledTask($task_id);
$logEntries = $task->LogEntries("ORDER BY log_date DESC");

$table = new DataListView($logEntries, "log_entries");
$table->column("Execution Date", "{log_date}", true, "width: 20%")
	  ->column("Status", "<a class='button' style='float: right' href='#' onclick='showTaskLog({log_entry_id}); return false'>Show Log</a> {status}", true);

$table->paginate = true;
$table->filter = true;
$table->sortable = true;
$table->excelFile = "scheduled_task_log.xls";
$table->emptyMessage = "There are no log files for this task.";

$tabs = ScheduledTaskManager::getTabs($task_id);

$script .= $table->writeScript();

$tabs->writeHTML();
?>
<div class='tab_border'>
<h4><?echo $task->format("{component:prettify}: {task_name}");?></h4>
<?
$table->drawView();
?>
<br/>
<a class='button' href='/action/scheduled_task/run_task?task_id=<?echo $task_id?>'>Run Now</a>
</div>
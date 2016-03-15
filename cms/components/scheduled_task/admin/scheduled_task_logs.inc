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

$log = new ScheduledTaskLogEntry();
$log->filter = new InclusionFilter("log_date");
$filterForm = new FilterForm($log);

$dateRangeFilter = new DateRangeFilterRenderer($filterForm, "log_date", "", "month");

		
$constraint = $filterForm->getConstraint();
$logEntries = $task->LogEntries("$constraint ORDER BY log_date DESC");

$table = new DataListView($logEntries, "log_entries");
$table->column("Execution Date", "{log_date}", true, "width: 20%")
	  ->column("Status", "<a class='button' style='float: right' href='#' onclick='showTaskLog({task_id}, {log_entry_id}, \"{log_date}\"); return false'>Show Log</a> {status}", true);

$table->paginate = true;
$table->pageSize = 20;
$table->filter = true;
$table->sortable = true;
$table->excelFile = "scheduled_task_log.xls";
$table->emptyMessage = "There are no log files for this task.";

$tabs = ScheduledTaskManager::getTabs($task_id);

$script .= $filterForm->writeScript();
$script .= $table->writeScript();
$script .= <<<ENDSCRIPT
<script type='text/javascript'>
function showTaskLog(task_id, log_entry_id, log_date)
{
	modalPopup("Task Log for " + log_date, "/action/scheduled_task/show_log?task_id=" + task_id + "&log_entry_id=" + log_entry_id, '800px', 'auto');
}
</script>
ENDSCRIPT;
$tabs->writeHTML();
?>
<div class='tab_border'>
<h4><?echo $task->format("{component:prettify}: {task_name}");?></h4>
<?
$filterForm->drawForm();
$table->drawView();
?>
<br/>
<a class='button' href='/action/scheduled_task/run_task?task_id=<?echo $task_id?>'>Run Now</a>
</div>
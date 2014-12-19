<?php
Fakoli::using("scheduled_task");
Fakoli::usingFeature("data_view");

$mgr = new ScheduledTaskManager();
$tasks = $mgr->getTasks();

$table = new DataListView($tasks, "scheduled_tasks");

$table->column("Task", "<a href='scheduled_task_form?task_id={task_id}'>{component:prettify}: {task_name}</a>", true, "width: 40%")
	  ->column("Last Run", "{LastRun.log_date}", true, "width: 15%")
	  ->column("Status", "", true, "width: 20%")
	  ->column("Next Run", "{next_run}", true, "width: 15%")
	  ->column("Active", "", true, "width: 10%");

$table->paginate = true;
$table->filter = true;
$table->sortable = true;
$table->emptyMessage = "No scheduled task workers have been registered";
$table->excelFile = "scheduled_tasks.xls";

$script .= $table->writeScript();

$table->drawView();

<?php
Fakoli::using("scheduled_task");
Fakoli::usingFeature("data_view");

$menu_item = "Scheduled Tasks";

$mgr = new ScheduledTaskManager();
$tasks = $mgr->getTasks();

$table = new DataListView($tasks, "scheduled_tasks");

$table->column("Task", "<a href='scheduled_task_form?task_id={task_id}'>{component:prettify}: {task_name}</a>", true, "width: 40%")
	  ->column("Last Run", "{LastRun.log_date}", true, "width: 15%")
	  ->column("Status", "{LastRun.status}", true, "width: 20%")
	  ->column("Next Run", "{next_run}", true, "width: 15%")
	  ->column("Active", "<img src='/fakoli/images/{active:on/off}.png' alt='{active:Yes/No}'/>", true, "width: 10%; text-align: center");

$table->paginate = true;
$table->filter = true;
$table->sortable = true;
$table->emptyMessage = "No scheduled task workers have been registered";
$table->excelFile = "scheduled_tasks.xls";

$script .= $table->writeScript();

$table->drawView();

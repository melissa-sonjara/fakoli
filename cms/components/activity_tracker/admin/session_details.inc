<?php
Fakoli::using("activity_tracker");
Fakoli::usingFeature("data_view");

$session_id = $_GET["session_id"];
$showImages = checkNumeric($_GET["showImages"]);

$title = "Session Details for $session_id";
if (!$showImages)
{
	$constraints .= "AND uri NOT LIKE '/action/image/%'";
}


$activity = Query::create(UserActivity, "WHERE session_id=:id $constraints ORDER BY activity_time ASC")
				 ->bind(":id", $session_id)
				 ->execute();

$table = new DataListView($activity, "activity_table");
$table->column("Date", "{activity_time}", true, "width: 15%")
	  ->column("Method", "{method}", true, "width: 10%")
	  ->column("URI", "{uri}", true);
	  
$table->filter = true;
$table->pageSize = 50;
$table->paginate = false;
$table->sortable = true;

$table->excelFile = "session_{$session_id}.xls";

$script .= $table->writeScript();
?>
<form method="GET" action="">
<input type='hidden' name='session_id' value="<?echo $session_id?>"/>
<input type="checkbox" class="checkbox" name="showImages" id="showImages" value="1"<?echo $showImages ? " checked='checked'" : ""?>/> Show Image Records&nbsp;&nbsp;
<input type="submit" class="button" value=" GO "/>
</form>
<?
$table->drawView();
?>
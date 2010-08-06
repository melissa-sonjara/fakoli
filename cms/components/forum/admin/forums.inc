<?php
Fakoli::using("forum");
Fakoli::usingFile("framework/data_view.inc");

$title = "Forums";

$fora = query(ForumSummary, "ORDER BY title");

$table = new DataListView($fora, "fora");
$table->column("Forum", "<a href='forum_form?forum_id={forum_id}'>{title}</a>{teaser}", true)
	  ->column("Topics", "{topics}", true)
	  ->column("Posts", "{posts}", true);
	  
$table->pageSize = 10;
$table->filter = true;
$table->sortable = true;

$table->writeScript();

require_once admin_begin_page;

$table->drawView();

?>
<br/>
<br/><a class='button' href='forum_form'> Create a Forum </a>
<?

require_once admin_end_page;
?>
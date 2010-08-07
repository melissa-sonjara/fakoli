<?php
Fakoli::using("forum");
Fakoli::usingFile("framework/data_view.inc");

$title = "Forums";

$menu_item = "Forums";

$fora = query(ForumSummary, "ORDER BY title");

$table = new DataListView($fora, "fora");
$table->column("Forum", "<a href='forum_form?forum_id={forum_id}'>{title}</a>{teaser}", true)
	  ->column("Topics", "{topics}", true)
	  ->column("Posts", "{posts}", true);
	  
$table->pageSize = 10;
$table->filter = true;
$table->sortable = true;

$table->writeScript();

$table->drawView();

?>
<br/>
<br/><a class='button' href='/admin/forum_form'> Create a Forum </a>
<?


?>
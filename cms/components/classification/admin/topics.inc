<?php

Fakoli::using("classification");
Fakoli::usingFile("framework/data_view.inc");

Fakoli::assertRole("admin");

$menu_item = "Topics";
$title = "Manage Topics";

$topics = query(Topic, "ORDER BY topic_id");

$table = new DataListView($topics, "topics");
$table->column("Topic", "<a href='topic_form?topic_id={topic_id}'>{topic}</a>", true);
$table->sortable = true;
$table->emptyMessage = "No topics have been created";

$script .= $table->writeScript();



$table->drawView();
?>
<br/>
<a href="topic_form" class="button"> Add a New Topic </a>
<br/>
<br/>
<?

?>
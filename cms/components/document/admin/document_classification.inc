<?php

Fakoli::using("document", "classification");
Fakoli::usingFile("framework/auto_form.inc");

require_once "include/document_tabs.inc";

Fakoli::assertRole("admin");

$menu_item = "Documents";

$publication_id = checkNumeric($_GET["publication_id"]);

$publication = new Document();

//$filter = new ExclusionFilter();
//$filter->add("password");

if (!checkRole("admin"))
{
	$filter->add("role");
}

$publication->filter = new InclusionFilter("publication_id", "title");

$tabs = publicationTabs($publication_id);

$form = new AutoForm($publication);
$form->hide("title");

$topics = query(Topic, "ORDER BY Topic");

$topicList = new CrossReferenceSelectFieldRenderer($form, "topics", "Topics", $topics, "<b>{topic}</b> ({details})", DocumentRegionXref);
$topicList->colspan = 2;

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->next();
	}
}
	
if ($publication_id)
{
	$publication->load($publication_id);
	$title = "Edit Document: {$publication->title}";
}
else
{
	$title =  "Add New Document";
}

$script = $form->writeScript();

require_once admin_begin_page;

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();
?>
</div><br/>
<?
require_once admin_end_page;
?>

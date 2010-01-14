<?php
require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../datamodel/publication.inc";
require_once "../datamodel/topic.inc";
require_once "../../framework/auto_form.inc";
require_once "include/publication_tabs.inc";

$menu_item = "Publications";

$publication_id = checkNumeric($_GET["publication_id"]);

$publication = new Publication();

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

$topicList = new CrossReferenceSelectFieldRenderer($form, "topics", "Topics", $topics, "<b>{topic}</b> ({details})", PublicationRegionXref);
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
	$title = "Edit Publication: {$publication->title}";
}
else
{
	$title =  "Add New Publication";
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

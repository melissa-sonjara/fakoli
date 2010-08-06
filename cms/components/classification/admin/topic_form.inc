<?php

Fakoli::using("classification");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

$menu_item = "Topics";
$topic_id = checkNumeric($_GET["topic_id"]);

$topic = new Topic();

$form = new AutoForm($topic);
$form->required("topic");
$form->allowDelete = true;
$redirect = "topics";
$form->button("Cancel", $redirect);

$groups = query(Group, "ORDER BY group_id"); 

$grpList = new CrossReferenceSelectFieldRenderer($form, "groups", "Associate Groups", $groups, "{title}", GroupTopicXref);
$grpList->colspan = 2;
$grpList->height = 120;

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("topics");	
	}
}

if ($topic_id)
{
	$topic->load($topic_id);
	$title = "Edit Topic Details for {$topic->topic}";
}
else
{
	$title = "Add a New Topic";
}

$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>

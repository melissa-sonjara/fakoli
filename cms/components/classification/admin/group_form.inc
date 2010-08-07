<?php

Fakoli::using("classification");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

$menu_item = "Groups";

$group_id = checkNumeric($_GET["group_id"]);

$group = new Group();

$form = new AutoForm($group);

$form->required("title");
$form->allowDelete = true;
$redirect = "groups";
$form->button("Cancel", $redirect);

$topics = query(Topic, "ORDER BY topic"); 
$topicList = new CrossReferenceSelectFieldRenderer($form, "topics", "Associate Topic", $topics, "{topic}", GroupTopicXref);
$topicList->colspan = 2;
$topicList->height = 120;

if ($method == "POST")
{

	if ($form->save())
	{
		redirect("groups");	
	}
}

if ($group_id)
{
	$group->load($group_id);
	$title = "Edit Page Details for {$group->title}";
}
else
{
	$title = "Add a New Group";
}

$script .= $form->writeScript();



$form->drawForm();


?>
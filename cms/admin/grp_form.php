<?php
require_once "../../include/config.inc";

require_once "../datamodel/topic.inc";
require_once "../datamodel/group.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";

$menu_item = "Groups";

$group_id = checkNumeric($_GET["group_id"]);

$group = new Group();

$form = new AutoForm($group);

$form->required("title");
$form->allowDelete = true;
$redirect = "grps.php";
$form->button("Cancel", $redirect);

$topics = query(Topic, "ORDER BY topic"); 
$topicList = new CrossReferenceSelectFieldRenderer($form, "topics", "Associate Topic", $topics, "{topic}", GroupTopicXref);
$topicList->colspan = 2;
$topicList->height = 120;

if ($method == "POST")
{

	if ($form->save())
	{
		redirect("grps.php");	
	}
}

if ($grp_id)
{
	$grp->load($grp_id);
	$title = "Edit Page Details for {$grp->title}";
}
else
{
	$title = "Add a New Group";
}

$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
<?php
require_once "../../include/config.inc";

require_once "../datamodel/topic.inc";
require_once "../datamodel/grp.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";

$menu_item = "Topics";
$topic_id = checkNumeric($_GET["topic_id"]);

$topic = new Topic();

$form = new AutoForm($topic);
$form->required("topic");
$form->allowDelete = true;
$redirect = "topics.php";
$form->button("Cancel", $redirect);

$grps = query(Grp, "ORDER BY grp_id"); 

$grpList = new CrossReferenceSelectFieldRenderer($form, "grps", "Associate Groups", $grps, "{title}", GrpTopicXref);
$grpList->colspan = 2;
$grpList->height = 120;

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("topics.php");	
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

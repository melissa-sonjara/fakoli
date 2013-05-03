<?php
/*
 * Allow user to edit a comment.
 * 
 * To do - Delete the comment_id in the xref table.
 */

Fakoli::using("comment", "settings", "email");
Fakoli::usingFeature("auto_form");

$comment_id = checkNumeric($_GET["comment_id"]);
$xref_class = mysql_escape_string($_GET["xref_class"]);
$key_name = mysql_escape_string($_GET["key_name"]);
$key_value = checkNumeric($_GET["key_value"]);
$xref_component = mysql_escape_string($_GET["xref_component"]);
global $user;

if ($xref_component) Fakoli::using($xref_component);

$comment = new Comment();

if ($comment_id)
{
	$comment->load($comment_id);
	$newPost = false;
}
else
{
	$comment->user_id = ($user) ? $user->get($user->primary_key) : 0;
	$comment->date_posted = today();
	$newPost = true;
}

$form = new AutoForm($comment, "POST", "/action/comment/comment_form?comment_id=$comment_id&xref_class=$xref_class&key_name=$key_name&key_value=$key_value&xref_component=$xref_component", "EditComment_form");
$form->alias("title", "Subject", "author", "Your Name");
$form->ajaxSubmit("function(result) {new Comment().commentFormResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->required("description");

// To do - make it configurable whether to include title?
$form->hide("title");

$posted = new DateTimeFieldRenderer($form);
$posted->template = "m/d/Y";
$form->override("date_posted", "Date Posted", $posted);
$form->readOnly("date_posted");

if(!$comment_id)
{
	$form->onSaveComplete = array(CommentView, onSaveNewComment);
	$form->data->set("xref_class", $xref_class);
	$form->data->set("key_name", $key_name);
	$form->data->set("key_value", $key_value);
}

if($comment->user_id)
{
	$mgr = new UserManager();
	$userClass = $mgr->getUserClass();
	$userSelect = new RelatedItemSelectFieldRenderer($form, "user_id", "Author", $userClass, "WHERE user_id={$comment->user_id}", "{first_name} {last_name}", "user_id");
	$form->readOnly("user_id");
	$form->hide("author");
}
else
{
	$form->hide("user_id");
	$form->readOnly("author");
}
	
$form->allowDelete = true;

if($method == "POST")
{
	//echo "<pre>"; print_r($_POST); echo "</pre>";
	if($form->save())
	{
		$confirmationMessage = $form->data->get("confirmation_message");
			
		Fakoli::end("OK|$confirmationMessage");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

echo $form->writeScript();

$form->drawForm();

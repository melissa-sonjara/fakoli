<?php
/*
 * Allow user to edit a comment.
 * 
 * To do - Delete the comment_id in the xref table.
 */

Fakoli::using("comment");
Fakoli::usingFeature("auto_form");

$comment_id = checkNumeric($_GET["comment_id"]);
$xref_class = mysql_escape_string($_GET["xref_class"]);

$comment = new Comment();

if($comment_id)
{
	$comment->load($comment_id);
}
else
{
	$comment->user_id = $user->get($user->primary_key);
	$comment->date_posted = today();
}

$form = new AutoForm($comment, "POST", "/action/comment/comment_form?comment_id=$comment_id&xref_class=$xref_class", "EditComment_form");
$form->ajaxSubmit("function(result) {new Comment().commentFormResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->required("description");
$form->data->set("xref_class", $xref_class);

// To do - make it configurable whether to include title?
$form->hide("title");

$posted = new DateTimeFieldRenderer($form);
$posted->template = "m/d/Y";
$form->override("date_posted", "Date Posted", $posted);
$form->readOnly("date_posted");

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
	if($form->save()) 
		ajaxReturn("OK");
	else
		ajaxReturn($form->msg);
}

echo $form->writeScript();

$form->drawForm();

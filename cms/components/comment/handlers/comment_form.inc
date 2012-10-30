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
        if ($user)
		{
			$comment->user_id = $user->get($user->primary_key);
			
		}
		else
		{
			$comment->user_id = 0;
			
		}
	
	$comment->date_posted = today();
	$newPost = true;
}

$form = new AutoForm($comment, "POST", "/action/comment/comment_form?comment_id=$comment_id&xref_class=$xref_class&key_name=$key_name&key_value=$key_value&xref_component=$xref_component", "EditComment_form");
$form->alias("title", "Subject", "author", "Your Name");
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
	//echo "<pre>"; print_r($_POST); echo "</pre>";
	if($form->save())
	{
		if ($newPost)
		{
			$xref = new $xref_class;
			$xref->set($form->data->primary_key, $form->data->get($form->data->primary_key));
			$xref->set($key_name, $key_value);
			$xref->save();
			if(!Settings::getValue("comment", "publish_automatically"))
			{
				sendEmailUsingEmailManager($comment, 'new_comment');
			}		
		}

		Fakoli::end("OK");
			
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

echo $form->writeScript();

$form->drawForm();

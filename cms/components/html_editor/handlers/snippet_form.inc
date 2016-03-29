<?php
Fakoli::using("html_editor");

$snippet_id = checkNumeric($_GET["snippet_id"]);

$snippet = new Snippet();

if ($snippet_id) 
{
	$snippet->load($snippet_id);
}
else
{
	$snippet->user_id = $user->getPrimaryKeyValue();
}	

$form = new AutoForm($snippet, "POST", "/action/html_editor/snippet_form?snippet_id={$snippet_id}");
$form->ajaxSubmit("function(result) { new SnippetManager().snippetSaved(result); }");
$form->required("snippet_name", "description");
$form->hide("user_id");

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end($form->data->snippet_id);
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

$script .= $form->writeScript();
$form->drawForm();
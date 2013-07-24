<?php
Fakoli::using("bookmark");
Fakoli::usingFeature("auto_form");

$bookmark_id = checkNumeric($_GET["bookmark_id"]);

$url = $_GET["url"];
$title = htmlSafe($_GET["title"]);

$bookmark = new Bookmark();

if ($bookmark_id)
{
	$bookmark->load($bookmark_id);
}
else
{
	$bookmark->url = $url;
	$bookmark->title = $title;
	$bookmark->user_id = $user->get($user->getPrimaryKey());
}

$form = new AutoForm($bookmark, "POST", "/action/bookmark/bookmark_popup?booknark_id={$bookmark_id}", "Bookmark_form");
$form->ajaxSubmit("function(result) {BookmarkManager.handleResponse(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");

$form->readonly("url");

$form->required("title");
$form->hide("user_id");
$form->allowDelete = true;

$categorySelect = new SelectFieldRenderer($form, "category", "Category");
$categorySelect->allowAddEntry();

if ($method == "POST")
{
	$_POST["user_id"] = $user->get($user->getPrimaryKey());
	
	if ($form->save())
	{
		Fakoli::end("OK");
	}
}

$script .= $form->writeScript();
$form->drawForm();
?>
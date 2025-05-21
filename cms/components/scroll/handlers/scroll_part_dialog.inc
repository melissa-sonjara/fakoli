<?php
Fakoli::using("scroll", "field_renderers");

if (!Settings::checkPermission("settings", "editor_roles"))
{
	Fakoli::end("Access Denied");
}

$scroll_id = checkNumeric($_GET["scroll_id"]);
$scroll_part_id = checkNumeric($_GET["scroll_part_id"]);
$content_type = checkIdentifier($_GET["content_type"]);

if ($scroll_part_id)
{
	$part = new ScrollPart($scroll_part_id);
	if ($scroll_id != $part->scroll_id)
	{
		throw new FakoliException("Parameter Mismatch");
	}
}
else
{
	if (!$scroll_id)
	{
		throw new FakoliException("Scroll Mismatch");
	}
	
	if ($content_type != 'HTML' && $content_type != 'Code')
	{
		throw new Exception("Invalid Content Type");
	}
	
	$part = new ScrollPart();
	$part->scroll_id = $scroll_id;
	$part->content_type = $content_type;
}

$form = new AutoForm($part, "POST", "/action/scroll/scroll_part_dialog?scroll_id={$scroll_id}&scroll_part_id={$scroll_part_id}&content_type={$content_type}");
$form->ajaxSubmit("function(response) { ScrollPartManager.editPartResponse(response); }");
$form->allowDelete = true;
$form->alias("css_class", "CSS Class");
$form->hide("content_type", "scroll_id", "sort_order");

VersionedContentManager::enableVersioning($form);

$form->onInsertPreProcess = function($form)
{
	$form->data->sort_order = Query::create('ScrollPart', "WHERE scroll_id=:s")
	->bind(":s", $form->data->scroll_id)
	->executeValue("MAX(sort_order) + 1");
};

if ($part->content_type == "HTML")
{
	$form->hide("php_code_file");
}
else
{
	$form->hide("content");
	$codeSelect = new CodeFileSelectFieldRenderer($form, "php_code_file", "PHP Code File", "modules");
}

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

$script .= $form->writeScript();
$form->drawForm();

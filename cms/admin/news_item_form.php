<?php
/* =======================================================================
 * Africa AI Site Management Application
 * Developed by Sonjara, Inc. for SRA, International
 * For further information please contact Andy Green at andy@sonjara.com
 * =======================================================================
 */

$page_role = "admin";

$menu_item = "News Items";

require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../datamodel/news_item.inc";
require_once "../../framework/auto_form.inc";
require_once "../datamodel/image.inc";
require_once "../components/field_renderers/image_select_field_renderer.inc";

$item = new NewsItem();

$form = new AutoForm($item);
$form->required("title");
$form->allowDelete = true;
$imageSelect = new ImageSelectFieldRenderer($form, "image_id");

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("news_items.php");
	}
	
	
}
else
{
	$news_item_id = checkNumeric($_GET["news_item_id"]);
	
	if ($news_item_id)
	{
		$form->load($news_item_id);
	}
}

if ($form->data->news_item_id)
{
	$title = "Edit News Item";
}
else
{
	$title =  "Add News Item";
}
$script = $form->writeScript();
$section = "What's New";

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>

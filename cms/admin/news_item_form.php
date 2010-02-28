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
require_once "../datamodel/site.inc";
require_once "../components/field_renderers/image_select_field_renderer.inc";

$item = new NewsItem();
$news_item_id = checkNumeric($_GET["news_item_id"]);
	
if ($news_item_id)
{
	$item->load($news_item_id);
}
else
{
	$item->created_date = now();
}

$form = new AutoForm($item);
$form->required("title");
$form->allowDelete = true;
$redirect = "news_items.php";
$form->button("Cancel", $redirect);
$imageSelect = new ImageSelectFieldRenderer($form, "image_id");
$form->alias("image_id", "Associated Image");
$form->readonly("created_date");
$articleTypeSelect = new SelectFieldRenderer($form, "article_type", "Article Type");
$articleTypeSelect->allowAddEntry();

$sites = query(Site, "ORDER BY site_name");

$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Sites", $sites, "site_name", NewsItemSiteXref);

$form->getRenderer("archive_date")->validator->required = false;

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("news_items.php");
	}
	
	
}
else
{
}

if ($form->data->news_item_id)
{
	$title = "Edit Article";
}
else
{
	$title =  "Add Article";

	if (!$news_item_id) $form->hide("created_date");
}

$script = $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>

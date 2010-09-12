<?php

Fakoli::using("article", "site", "image", "field_renderers");
Fakoli::usingFile("framework/auto_form.inc");

$article = new Article();
$article_id = checkNumeric($_GET["article_id"]);
	
if ($article_id)
{
	$article->load($article_id);
}
else
{
	$article->created_date = now();
}

$form = new AutoForm($article);
$form->required("title");
$form->allowDelete = true;
$redirect = "/admin/articles";
$form->button("Cancel", $redirect);
$imageSelect = new ImageSelectFieldRenderer($form, "image_id");
$form->alias("image_id", "Associated Image");
$form->readonly("created_date");
$form->hide("author_id"); //TODO - allow author selection/re-assignment?
$articleTypeSelect = new SelectFieldRenderer($form, "article_type", "Article Type");
$articleTypeSelect->allowAddEntry();

$sites = query(Site, "ORDER BY site_name");

$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Sites", $sites, "site_name", ArticleSiteXref);

$form->getRenderer("archive_date")->validator->required = false;

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/articles");
	}
	
	
}
else
{
}

if ($form->data->article_id)
{
	$title = "Edit Article";
}
else
{
	$title =  "Add Article";

	if (!$article_id) $form->hide("created_date");
}

$script = $form->writeScript();



$form->drawForm();


?>

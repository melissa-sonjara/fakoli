<?php
require_once "../../include/config.inc";

require_once "../datamodel/page.inc";
require_once "../datamodel/menus.inc";
require_once "../datamodel/site.inc";

require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";
require_once "../../framework/tree.inc";
require_once "include/page_tabs.inc";
require_once "../components/field_renderers/code_file_select_field_renderer.inc";
require_once "../components/field_renderers/template_select_field_renderer.inc";


$menu_item = "Pages";

$page_id = checkNumeric($_GET["page_id"]);

$page = new Page();
$page->filter = new ExclusionFilter("menu_id");

$page->created_date = date("Y-m-d");

$page->archive_date = strftime('%Y-%m-%d', '0000-00-00');
$form = new AutoForm($page);
$form->alias("menu_id", "Menu");
$form->markRequiredFields = true;

$form->required("page_title", "identifier");
$form->regexp("identifier", "^[\\w_\\-]+$", "Identifier must consist of letters, numbers, underscores and dashes only.");

$form->readonly("created_date");
$form->unique("identifier", "A page on the site is already using this name. Please pick a new identifier.");
$form->allowDelete = true;

$codeSelect = new CodeFileSelectFieldRenderer($form, "php_code_file", "PHP Code File", "modules");
$templateSelect = new TemplateSelectFieldRenderer($form, "template", "Template", "templates");


$roleList = new CheckListFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());

$siteSelect = new RelatedItemSelectFieldRenderer($form, "site_id", "Site", Site, "ORDER BY site_name", "site_name", "site_id");


$tabs = pageTabs($page_id);

if ($method == "POST")
{

	if ($form->save())
	{
	    if ($form->deleted)
		{
			redirect("pages.php");
		}
		
		$tabs->queryString = "page_id={$form->data->page_id}";
		$tabs->next();
	}
}

if ($page_id)
{
	$page->load($page_id);
	$title = "Edit Page Details for {$page->page_title}";
}
else
{
	$title = "Add a New Page";
}

$redirect = "pages.php";
$form->button("Cancel", $redirect);


$roleOptions = array(
	""				 => "All",
	"admin"			=>	"Admin",
	"admin,member" 	=>	"Admin/Member"
	
);

$script .= $form->writeScript();

require_once admin_begin_page;

$tabs->writeHTML();
?>
<div id="form"  style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$form->drawForm();
?>
</div>
<?

require_once admin_end_page;
?>

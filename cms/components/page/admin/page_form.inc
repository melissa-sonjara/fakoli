<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::using("page", "menu", "site", "field_renderers", "role", "taxonomy", "versioned_content");

Fakoli::usingFeature("auto_form", "tree");

Fakoli::assertRole("admin,editor");

$menu_item = "Pages";

$page_id = checkNumeric($_GET["page_id"]);

$cmspage = new Page();
$cmspage->filter = new ExclusionFilter("menu_id");

if ($page_id)
{
	$cmspage->load($page_id);
	$title = "Edit Page Details for {$cmspage->page_title}";
}
else
{
	$title = "Add a New Page";
}

$cmspage->created_date = date("Y-m-d");

$form = new AutoForm($cmspage);
$form->alias("menu_id", "Menu");
$form->markRequiredFields = true;

$form->required("page_title", "identifier");
$form->regexp("identifier", "^[\\w_\\-]+$", "Identifier must consist of letters, numbers, underscores and dashes only.");

$form->readonly("created_date");
$form->unique("identifier", "A page on the site is already using this name. Please pick a new identifier.");
$form->allowDelete = true;

$codeSelect = new CodeFileSelectFieldRenderer($form, "php_code_file", "PHP Code File", "modules");
$templateSelect = new TemplateSelectFieldRenderer($form, "template", "Template", "site");
$form->annotate("template", "Fakoli 3 projects use Section Content to set template");

TaxonomyManager::addTaxonomyFieldRenderers($form);

$roleList = new CheckListFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());

$siteSelect = new RelatedItemSelectFieldRenderer($form, "site_id", "Site", 'Site', "ORDER BY site_name", "site_name", "site_id");

$sectionContentField = new SectionContentPropertiesFieldRenderer($form);

$script .= <<<ENDSCRIPT
<script>
function onPageIdentifierUpdated(field)
{
	sectionContentPropertyManager.updateIdentifier(field.value);
}
</script>
ENDSCRIPT;
$form->getRenderer("identifier")->onChange = "onPageIdentifierUpdated";

VersionedContentManager::enableVersioning($form);


$tabs = PageManager::pageTabs($page_id);
$tabs->page = "page_form";

if ($method == "POST")
{

	if ($form->save())
	{
	    if ($form->deleted)
		{
			redirect("/admin/pages");
		}
		
		$tabs->queryString = "page_id={$form->data->page_id}";
		$tabs->next();
	}
}

$redirect = "/admin/pages";
$form->button("Cancel", $redirect);


$roleOptions = array(
	""				 => "All",
	"admin"			=>	"Admin",
	"admin,member" 	=>	"Admin/Member"
	
);

$script .= $form->writeScript();



$tabs->writeHTML();
?>
<div class='tab_border'>
<?
$form->drawForm();
?>
</div>
<?


?>

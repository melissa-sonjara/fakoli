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

if (!Settings::checkPermission("settings", "editor_roles"))
{
	Fakoli::end("Access Denied");
}

$page_id = checkNumeric($_GET["page_id"]);

$cmspage = new Page();
$cmspage->filter = new ExclusionFilter("menu_id", "identifier", "site_id", "template", "php_code_file");
$cmspage->created_date = date("Y-m-d");

if ($page_id)
{
	$cmspage->load($page_id);
}

$form = new AutoForm($cmspage, "POST", "/action/page/edit?page_id={$page_id}");
$form->markRequiredFields = true;

$form->required("page_title");

$form->readonly("created_date");
$form->allowDelete = true;

//$codeSelect = new CodeFileSelectFieldRenderer($form, "php_code_file", "PHP Code File", "modules");
//$templateSelect = new TemplateSelectFieldRenderer($form, "template", "Template", "site");
$form->annotate("template", "Fakoli 3 projects use Section Content to set template");
$form->ajaxSubmit("function(result) {new PageManager().editResult(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");

TaxonomyManager::addTaxonomyFieldRenderers($form);

$roleList = new CheckListFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());

VersionedContentManager::enableVersioning($form);


$redirect = "/admin/pages";
$form->button("Cancel", "new PageManager().closeDialog(); return false", null, true);

if ($method == "POST")
{

	if ($form->save())
	{
	    if ($form->deleted)
		{
			Fakoli::end("DELETED");
		}
		else
		{
			Fakoli::end("OK");
		}
	}
}

$script .= $form->writeScript();
$form->drawForm();
?>
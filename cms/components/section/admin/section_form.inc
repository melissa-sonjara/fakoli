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

Fakoli::using("section", "field_renderers", "site");
Fakoli::usingFeature("auto_form");

require_once "include/section_tabs.inc";

$menu_item = "Sections";

$section_id = checkNumeric($_GET["section_id"]);

$section = new Section();

if ($section_id)
{
	$section->load($section_id);
}

$form = new AutoForm($section);

$form->alias("section", "Section Identifier");

$templateSelect = new TemplateSelectFieldRenderer($form, "default_template", "Default Template", "templates");

$roleList = new CheckListFieldRenderer($form, "default_role", "Default Role", SiteRole::getRolesArray());

$siteSelect = new RelatedItemSelectFieldRenderer($form, "site_id", "Site", Site, "ORDER BY site_name", "site_name", "site_id");
$identifierSelect = new CMSItemSelectFieldRenderer($form, "default_page", "Default Page", null);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/sections");
	}
}

$script .= $form->writeScript();

$tabs = sectionTabs($section_id);

$tabs->writeHTML();
?>
<div id="form"  style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$form->drawForm();
?>
</div>
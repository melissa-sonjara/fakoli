<?php
/**************************************************************

 Copyright (c) 2016 Sonjara, Inc

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

Fakoli::using("scroll", "menu", "site", "field_renderers", "role", "taxonomy", "versioned_content");

Fakoli::usingFeature("auto_form", "tree");

Fakoli::assertRole("admin,editor");

$menu_item = "Scrolls";

$scroll_id = checkNumeric($_GET["scroll_id"]);

$scroll = new Scroll();
$scroll->filter = new ExclusionFilter("menu_id");

if ($scroll_id)
{
	$scroll->load($scroll_id);
	$title = "Edit Scroll Details for {$scroll->scroll_title}";
}
else
{
	$title = "Add a New Scroll";
}

$scroll->created_date = date("Y-m-d");

$form = new AutoForm($scroll);

$form->required("title", "identifier");
$form->regexp("identifier", "^[\\w_\\-]+$", "Identifier must consist of letters, numbers, underscores and dashes only.");

$form->readonly("created_date");
$form->unique("identifier", "That identifier is already in use. Please pick a new identifier.");
$form->allowDelete = ($scroll_id && $scroll->countParts());

TaxonomyManager::addTaxonomyFieldRenderers($form);

$siteSelect = new RelatedItemSelectFieldRenderer($form, "site_id", "Site", Site, "ORDER BY site_name", "site_name", "site_id");

//VersionedContentManager::enableVersioning($form);


$tabs = ScrollManager::scrollTabs($scroll_id);
$tabs->page = "scroll_form";

if ($method == "POST")
{

	if ($form->save())
	{
	    if ($form->deleted)
		{
			redirect("/admin/scrolls");
		}
		
		$tabs->queryString = "scroll_id={$form->data->scroll_id}";
		$tabs->next();
	}
}

$redirect = "/admin/scrolls";
$form->button("Cancel", $redirect);

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

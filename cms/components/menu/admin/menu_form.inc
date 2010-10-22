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

Fakoli::using("menu");
Fakoli::using("site");

Fakoli::assertRole("admin");

Fakoli::usingFile("framework/auto_form.inc");

require_once "include/menu_tabs.inc";

$menu_item = "Menus";

$menu_id = checkNumeric($_GET["menu_id"]);

$menu = new Menu();

$tabs = menuTabs($menu_id);
$tabs->page = "menu_form";

$form = new AutoForm($menu);

$form->required("title");
$form->allowDelete = true;
$redirect = "/admin/menus";
$form->button("Cancel", $redirect);
$siteSelect = new RelatedItemSelectFieldRenderer($form, "site_id", "Site", Site, "order by site_name", "site_name", "site_id");
if ($method == "POST")
{

	if ($form->save())
	{
		redirect("/admin/menus");	
	}
}

if ($menu_id)
{
	$menu->load($menu_id);
	$title = "Edit Menu Details for {$menu->name}";
}
else
{
	$title = "Add a New Menu";
}

$script .= $form->writeScript();



$tabs->writeHTML();
?>
<div id="form" style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$form->drawForm();
?>
</div>
<?

?>
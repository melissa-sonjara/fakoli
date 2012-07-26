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

Fakoli::using("module", "menu", "field_renderers");
Fakoli::usingFile("framework/auto_form.inc");

$menu_item = "Modules";
$module_id = checkNumeric($_GET["module_id"]);
$module = new Module();

$module->filter = new InclusionFilter("module_id", "title", "content_type", "css_class", "menu_id", "menu_parameters", "global", "global_position");
$module->content_type = "Menu";

$form = new AutoForm($module);
$form->required("title");
$form->hide("content_type");
$form->alias("css_class", "CSS Class");

$form->allowDelete = true;
$redirect = "/admin/modules";
$form->button("Cancel", $redirect);

$menuSelect = new RelatedItemSelectFieldRenderer($form, "menu_id", "Menu", Menu, "ORDER BY name", "name", "menu_id");

if ($method == "POST")
{	
	if ($form->save())
	{
		redirect("/admin/modules");	
	}
}

if ($module_id)
{
	$module->load($module_id);
	$title = "Edit Module: {$module->title}";
	$form->button("Run the Module", "/admin/module_preview?module_id=$module_id");
}
else
{
	$title = "Add a New Menu Module";
}

//$script .= $form->writeScript();



$form->drawForm();


?>

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

Fakoli::using("module");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

$menu_item = "Modules";
$module_id = checkNumeric($_GET["module_id"]);

$module = new Module();
$module->content_type = HTML;

$form = new AutoForm($module);
//$form->override("template", $label = "HTML", $renderer = "HTML");
$form->override("template", $label = "HTML", new HTMLFieldRenderer($form));

$form->required("title");
$form->hide("content_type","ord_by","query_constraint","num_items", "php_code_file", "menu_id", "menu_parameters", "group_id");
$form->alias("css_class", "CSS Class");
$form->allowDelete = true;
$redirect = "/admin/modules";
$form->button("Cancel", $redirect);

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
	$module->content_type = HTML;
	$title = "Edit Module: {$module->title}";
	$form->override("template", $label = "HTML", new HTMLFieldRenderer($form));
	$form->button("Run the Module", "/admin/module_preview?module_id=$module_id");
}
else
{
	$title = "Add a New HTML Module";
}

$script .= $form->writeScript();



$form->drawForm();


?>

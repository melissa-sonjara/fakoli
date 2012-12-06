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

Fakoli::using("site");
Fakoli::using("field_renderers");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

$menu_item = "Sites";

$site_id = checkNumeric($_GET["site_id"]);

$site = new Site();

$form = new AutoForm($site);

$form->required("title");
$form->allowDelete = true;
$redirect = "/admin/sites";
$form->button("Cancel", $redirect);
$form->annotate("meta_tag_description", "Default content for the description meta-tag");
$form->annotate("meta_tag_keyword", "Default content for the keywords meta-tag");

$templateSelect       = new TemplateSelectFieldRenderer($form, "default_template", "Default Template", "templates");
$printTemplateSelect  = new TemplateSelectFieldRenderer($form, "print_template", "Print Template", "templates");
$popupTemplateSelect  = new TemplateSelectFieldRenderer($form, "popup_template", "Popup Template", "templates");
$mobileTemplateSelect = new TemplateSelectFieldRenderer($form, "mobile_template", "Mobile Template", "templates");

if ($method == "POST")
{

	if ($form->save())
	{
		redirect($redirect);	
	}
}

if ($site_id)
{
	$site->load($site_id);
	$title = "Edit Site Details for {$site->site_name}";
}
else
{
	$title = "Add a New Site";
}

$script .= $form->writeScript();



$form->drawForm();


?>
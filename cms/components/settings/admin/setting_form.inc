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

Fakoli::using("settings", "field_renderers");
Fakoli::usingFeature("auto_form");

$settings_id = checkNumeric($_GET["settings_id"]);

$setting = new Settings();

$menu_item = "Define Settings Options";

if ($settings_id)
{
	$setting->load($settings_id);
	$title = "Edit Setting";
}
else
{
	$title = "Add Setting";
}

$form = new AutoForm($setting);

$form->readOnly("component");
$form->hide("value");
$form->button("Cancel", "/admin/setting_list");
$form->annotate("name", "Please use a code-style name with lowercase letters and underscores between words (e.g., output_path). <b><i>Warning</b></i> Changes to the name will require code changes.");
$form->annotate("options", "Option list for select or checklist field renderers or name of custom field renderer (e.g., FileUploadFieldRenderer). <b><i>Warning</b></i>: changing the order of options will change the selected value on the Application Settings form.");
$form->annotate("name", "Name changes must be accompanied by code updates to the function that sets the default value.");
$typeSelect = new SelectFieldRenderer($form, "field_type", "Field Type", Settings::$fieldTypes);

if ($method == "POST")
{
	if($form->save())
		redirect("/admin/setting_list");
}

$script .= $form->writeScript();

?>
<h4>This features allows you to customize the system's application setting options. It is designed for use by systems administrators. CMS Users can modify <a href="/admin/settings">Application Settings</a> under CMS Site Configuration.</h4>
<?php
$form->drawForm();
?>
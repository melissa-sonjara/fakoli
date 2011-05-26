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

/*
 * For admin to define text that appears on
 * various pages in the site.
 * 
 * Janice Gallant for Sonjara, Inc.
 * 
 * 09/03/2010
 */


Fakoli::using("text_lookup", "email");
Fakoli::usingFeature("auto_form");
Fakoli::usingFile("framework/tab_bar.inc");

$text_id = checkNumeric($_GET["text_id"]);

$menu_item = "Text Lookup";

$text = new TextLookup();

if ($text_id)
{
	$text->load($text_id);
}


$redirect = "/admin/text_lookup_list";

$form = new AutoForm($text);
$form->unique("code", "Text already exists with that code.");

$classes = ComponentManager::fireEvent("EnumerateDataItemClasses", $classes);
if (count($classes) > 0)
	$classNameList = array_combine($classes, $classes);

$classSelect = new SelectFieldRenderer($form, "class_name", "Class Name", $classNameList);
$classSelect->allowAddEntry();
$form->annotate("class_name", "If merge codes are used, identify the class name that can access the values.");
$form->button("Cancel", $redirect);
$form->allowDelete = true;
$form->onDelete = onDeleteCascade;

if($text_id)
	$form->annotate("code", "Code field changes will require changes to existing lookups in the code base.");
else
	$form->annotate("code", "Please provide the code as all lowercase letters and link words with an underscore (e.g., my_code)");

$tabs = TextLookupManager::getTabs($text_id);
	
if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/text_translations?text_id={$form->data->text_id}");
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border">
<p>This form allows you to edit text that appears in the website. The text is retrieved for display on a web page using the code name given here.</p>
<p><a class="button" href="#" onclick="new TextLookup().showAdvancedFeatures(); return false">Advanced Features</a></p>
<?php

function onDeleteCascade(&$text)
{
	TextLookupManager::deleteText($text);	
}

$form->drawForm();
?></div>
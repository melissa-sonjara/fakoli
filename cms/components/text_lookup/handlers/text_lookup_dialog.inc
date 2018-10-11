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
else
{
	Fakoli::end("Missing Parameter");
}

if (!Settings::getValue("settings", "enable_inline_editing") || !Settings::checkPermission("settings", "editor_roles"))
{
	Fakoli::end("Permission Denied");
}

$form = new AutoForm($text, "POST", "/action/text_lookup/text_lookup_dialog?text_id={$text_id}");
$form->ajaxSubmit("function(response) { TextLookupManager.editTextResponse(response); }");

$form->hide("category", "editable", "enabled");
$form->readonly("code", "class_name");
if (!$text->class_name) $form->hide("class_name");

$form->unique("code", "Text already exists with that code.");
$form->required("code");

VersionedContentManager::enableVersioning($form);

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

$script .= $form->writeScript();
$form->drawForm();

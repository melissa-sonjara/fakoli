<?php
/**************************************************************

 Copyright (c) 2014 Sonjara, Inc

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


Fakoli::using("tool_hints");
Fakoli::usingFeature("auto_form");

$hint_id = checkNumeric($_GET["hint_id"]);

$menu_item = "Tool Hints";

$hint = new ToolHint();

if ($hint_id)
{
	$hint->load($hint_id);
}

$redirect = "/admin/tool_hints";

$form = new AutoForm($hint);
$form->unique("code", "A tool hint already exists with that code.");
$form->required("code", "title", "hint");

$form->button("Cancel", $redirect);
$form->allowDelete = true;

$roleList = new CheckListFieldRenderer($form, "read_access", "Read Access", SiteRole::getRolesArray());

if($hint_id)
{
	$form->annotate("code", "Code field changes will require changes to existing lookups in the code base.");
}
else
{
	$form->annotate("code", "Please provide the code as all lowercase letters and link words with an underscore (e.g., my_code)");
}

$form->annotate("title", "The title to display in the tool hint popup.");

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();
$form->drawForm();
?>
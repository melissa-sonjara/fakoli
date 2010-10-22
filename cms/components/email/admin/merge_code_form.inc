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
 * Title: email_code_form
 * 
 * Description: add/update/delete view for email message
 * templates fields. For developers only.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 8/13/10
 *
***************************************************************/


Fakoli::using("email");
Fakoli::usingFeature("auto_form");

$merge_code_id = checkNumeric($_GET["merge_code_id"]);

$mergeCode = new MergeCode();

if ($merge_code_id)
{
	$mergeCode->load($merge_code_id);
}

$form = new AutoForm($mergeCode);

$redirect = "/admin/merge_code_list";
$form->required("name", "class_name", "field");
$form->allowDelete = true;
$form->annotate("name", "The field name that will be used in the email subject or message (e.g., employee_name, due_date, amount)");
$form->annotate("function", "Whether the map is a function rather than relation");
$form->annotate("map", "The path from the field name to the database values");
$form->annotate("class_name", "The name of the dataitem class that can send the message (get the data values)");
$form->button("Cancel", $redirect);
$form->unique("name", "This name already exists. Please choose a different one.");

if ($method == "POST")
{
	if($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();

echo "<p>Merge code maps define the relation or function from the mail sending class to the value in the form FunctionName, RelationName.fieldname, or RelationName.functionName or callbackFunctionName. The final value needed must be returned by the final value in the map, whether a field or function.";
$form->drawForm();
?>
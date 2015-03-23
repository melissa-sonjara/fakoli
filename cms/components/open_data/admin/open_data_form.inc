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


Fakoli::using("OpenData");
Fakoli::usingFeature("data_view");

OpenDataManager::getOpenDataClasses();

$menu_item = "Open Data";
$open_data_id = checkNumeric($_GET["open_data_id"]);
$class = checkIdentifier($_GET["class"]);
$field = checkIdentifier($_GET["field"]);

$openData = new OpenData();

if ($open_data_id)
{
	$openData->load($open_data_id);
	$title = "Edit Open Data Details for {$openData->field}";
}
else
{
	$obj = new $class;
	
	$openData->class = $class;
	$openData->field = $field;
	$openData->field_type = $obj->getType($field);
}

$form = new AutoForm($openData);
$form->allowDelete = true;
if ($method == "POST")
{
	if ($form->save())
	{
		redirect("open_data_items");
	}
}
$form->alias("private", "Personally Identifiable Information");
$form->alias("protected", "Otherwise Protected");

$opendataselect = new SelectFieldRenderer($form, "open_data_level", "Open Data Level:", OpenData::$open_data_level);
$form->drawForm();


?>
</div>
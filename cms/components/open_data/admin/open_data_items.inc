<?php
/**************************************************************

Copyright (c) 2015 Sonjara, Inc

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

Fakoli::using("open_data");
Fakoli::usingFeature("grouped_data_view");

$menu_item = "Open Data";

$classes = OpenDataManager::getOpenDataClasses();

$savedOpenData = GroupedQuery::create(OpenData, "ORDER BY field", "class")->execute();

$openData = array();

/*
 * Build an array of context help objects grouped by class.
* If we have one saved, then use that; if not, create an
* new one and set the field and class name but don't save it.
* Help items are only saved when the user enters help on the form.
*/
if(count($classes) > 0)
{
	foreach($classes as $class)
	{
		$classObj = new $class();
		$prettyClassName = $classObj->prettifyClassName();
		$fields = $classObj->getFields();
		$savedFields = $savedOpenData[$class];
		if(count($savedFields) == 0) $savedFields = array();
		if(is_array($classObj->additionalFields))
			$fields = array_merge($fields, array_combine($classObj->additionalFields, $classObj->additionalFields));


		
		$savedFields = reindexList($savedFields, "field");

		if(count($fields) > 0)
		{
			foreach($fields as $fieldName => $fieldType)
			{
				if(!array_key_exists($fieldName, $savedFields))
				{
					$openDataRecord = new OpenData();
					$openDataRecord->class = $class;
					$openDataRecord->field = $fieldName;
				}
				else
				{
					$openDataRecord = $savedFields[$fieldName];
				}

				$openDataRecord->pretty_class_name = $prettyClassName;
				array_push($openData, $openDataRecord);
			}
		}
	}
}

$openData = OpenDataHelper::sortItems($openData);
$groupedOpenData = regroupList($openData, "pretty_class_name");

$table = new GroupedDataListView($groupedOpenData, "open_data");
$table->column("Field", "{OpenDataHelper::formatFieldLink}")
	  ->column("Open Data Level", "{open_data_level}", true)
	  ->column("Source", "{source}", true)
	  ->column("Data Standard", "{data_standard}", true)
	  ->column("Ignored", array($this, Ignored), false, "text-align: center");

$table->mode = "tree";
$table->cssStyle = "width: 90%";

foreach(array_keys($groupedOpenData) as $class_name)
{
	$table->group($class_name, $class_name);
}

$table->emptyMessage = "No open data meta tags have been created yet.";

$script .= $table->writeScript();

$table->drawView();
?>
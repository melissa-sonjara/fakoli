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
 * 
 * Title:  context_help_list.php
 * 
 * Description: Displays a list of the features for which
 * context help can be written.
 * 
 * 1) Get all the valid files from the datamodel directory
 * 
 * 2) Read each file line by line to gather the names of 
 * each class. Automatically eliminate classes we know are
 * irrelevant for help: join classes and status forms. Check
 * for a comment on the class declaration line, "// no help"
 * and don't include those classes
 * 
 * 3) Given an array of class names, create an instance of
 * each class to get the following:
 * 
 * a) the fields array for the list of fields for which
 * help can be provided
 * 
 * b) the "additionalFields" array which provides the list 
 * of supplemental or pseudo fields associated with a 
 * feature/script for which help can be created.
 * 
 * c) the prettified class name which we want to display
 * to the user when presenting classes for which help
 * can be defined
 * 
 * 4) Retrieve from the db, all the defined help, grouped
 * by class name. Match the classes for which help is 
 * available with the classes array. If help is found,
 * match the help field with the field in the array for
 * that class.
 * 
 * 5) Display a table with the list of classes, fields
 * in the class and a checkbox to indicate if a help
 * record exists for that class/field. The table
 * enables the user to click on a class/field to
 * edit help or create a help record.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/09
 * 
 */

Fakoli::using("context_help");
Fakoli::usingFeature("auto_form", "grouped_data_view");

$title = "Context-Sensitive Help";

$menu_item = "Context Help";
	
$classes = array();
$classes = ComponentManager::fireEvent("EnumerateContextHelpClasses", $classes);

$savedHelp = groupedQuery('ContextHelp', "", "class_name");
$contextHelps = array();

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
		$savedFields = $savedHelp[$class];
		if(!$savedFields || count($savedFields) == 0) $savedFields = array();
		if(is_array($classObj->additionalFields))
			$fields = array_merge($fields, array_combine($classObj->additionalFields, $classObj->additionalFields));
		
		$savedFields = reindexList($savedFields, "field");
		
		if(count($fields) > 0)
		{
			foreach($fields as $fieldName => $fieldType)
			{
				if(!array_key_exists($fieldName, $savedFields))
				{
					$contextHelp = new ContextHelp();
					$contextHelp->class_name = $class;
					$contextHelp->field = $fieldName;
				}
				else
				{
					$contextHelp = $savedFields[$fieldName];
				}
				
				$contextHelp->pretty_class_name = $prettyClassName;
				array_push($contextHelps, $contextHelp);			
			}
		}
	}
}

$contextHelps = sortItems($contextHelps, "compareClasses");
$groupedHelps = regroupList($contextHelps, "pretty_class_name");

$table = new GroupedDataListView($groupedHelps, "ContextHelp");
$table->column("Field", 'formatContextHelpFormLink')
		  ->column("Title", "{title}", true)
			->column("Help", "{help}", true)
			;
		  
$table->mode = "tree";
$table->cssStyle = "width: 90%";

foreach(array_keys($groupedHelps) as $class_name)
{
	$table->group($class_name, $class_name);
}

$script = $table->writeScript();

?><p>Using the list below you can provide content-sensitive help and publication criteria
popup text for use on forms throughout the site.</p>
<? 
$table->drawView();

function formatContextHelpFormLink($contextHelp)
{
	if($contextHelp->help_id) 
		$param = "help_id={$contextHelp->help_id}";
	else
		$param = "class_name={$contextHelp->class_name}&field={$contextHelp->field}";
	$html = "<a href=\"/admin/context_help_form?$param\">{$contextHelp->field}</a>\n";
	
	return $html;
}

/*
 * compare
 * 
 * Orders items by class name, case insensitive 
 * 
 * @param itemA 2-D array containing subject string
 * @param itemB 2-D array containing subject string
 */
function compareClasses($itemA, $itemB)
{
	if (strcasecmp($itemA->pretty_class_name, $itemB->pretty_class_name) == 0) 
        	return 1;
     elseif (strcasecmp($itemA->pretty_class_name, $itemB->pretty_class_name) < 0)
    	return -1;
    else
    	return 1;
}

function sortItems($items, $compare)
{
	if(is_array($items))
	{
		usort($items, $compare);
	}
	return $items;
}

?>

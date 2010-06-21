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
*
*/**************************************************************
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
 ****************************************************************/

require_once  "../../../include/config.inc";
require_once  $config['homedir']."/include/permissions.inc";
require_once  $config['homedir']."/datamodel/context_help.inc";
require_once  $config['homedir']."/framework/auto_form.inc";
require_once  $config['homedir']."/framework/tree.inc";



$title = "Context-Sensitive Help";
$page_role = "admin";

	
$includeFiles = array();	
$classes = getClassesFromDataModel($includeFiles);
foreach($includeFiles as $include)
{
	require_once $include;
}

$sort = checkNumeric($_GET["sort"]);
if(!$sort)
	$sort = 1;

//$fieldHelpItems = getHelpFromDb($classes, $sort);
$classTree = buildClassHelpTree($classes);
$script .= "<link type='text/css' rel='stylesheet' href='/css/tree.css'/>";

include_once admin_begin_page;

?><p>Using the list below you can provide content-sensitive help and publication criteria
popup text for use on forms throughout the site.</p>
<? 
$classTree->writeHTML();

include_once admin_end_page;

/*
 * Search the scripts in the datamodel folder for classes for which
 * help can be written.
 */
function getClassesFromDataModel(&$includeFiles)
{
	global $config;
	$lines = array();
	$classes = array();
	
	$path = $config["homedir"]."/datamodel";
	
	if ($handle = opendir($path)) 
	{
    	while (false !== ($file = readdir($handle))) 
    	{
    		// include *.inc and exclude ".", "..", ".bak", and ".CVS"
  	       if (preg_match('/.inc$/i', $file) && !preg_match('/(.bak$)|(^CVS$)|(^\.{1,2}$)/i', $file)) 
 	       {
 	       		$filename = "{$path}/$file";
 	       		array_push($includeFiles, $filename);
  				getHelpClasses(file_get_contents($filename), $classes);
     	   }        	
    	}

    closedir($handle);
	}
 
	return $classes;
}

/*
 * Search each line for the word "DataItem". If found, get the
 * class name at the beginning of the line.
 * 
 * Automatically eliminate Jn classes and FormStatus classes.
 * Other classes are eliminated if a comment follows the
 * class
 */
function getHelpClasses($file, &$classes)
{
	$matches = array();
	
	$count = preg_match_all("/^\\s*class\\s+(\\w+?)\\s+extends\\s+DataItem(?!.*no help)/mi", $file, $matches, PREG_PATTERN_ORDER);
	$classes = array_merge($classes, preg_grep("/(Xref|Jn|FormStatus)$/", $matches[1], PREG_GREP_INVERT));
}

/*
 * Get all the help records from the db, grouped by class. For
 * each help record, match it with the class/field we got from 
 * the datamodel.
 * 
 * For each class name we got from datamodel include file,
 * we need to get the class name we will dipslay (pretty_class_name,
 * if it exists or use class name from the file), field name,
 * help key, if exists. We put all this into an array and sort
 * by class name, then field name, before building the tree. 
 */

function buildClassHelpTree($classes)
{
	$classTree = new TreeControl("class_tree", "", "tree", true, 600);
	$classTree->height = "350";
	$classTree->indent = "0px";
	
	$fieldHelpItems = getHelpFromDb($classes);
	
	foreach($fieldHelpItems as $classItems)
	{
		if(is_array($classItems))
		{
			$class = $classItems[0];
		
			$classNode = new TreeNode("class_$class->class_name", prettify($class->class_name), null, false, "bare_node_closed", "bare_node_open"); 
		
			foreach($classItems as $item)
			{			
				$fieldNode = new TreeNode("field_{$item->field}", $item->field, null, false, "bare_node_closed", 
					"bare_node_open", "context_help_form.php?help_key=$item->help_key&field=$item->field&class=$item->class_name&prettyClassName=$class->pretty_class_name");
				$fieldNode->leafStyle = $item->help_key ? "checked_node_leaf" : "unchecked_node_leaf";
				$classNode->add($fieldNode);
				
			}
			$classTree->add($classNode);		
		}
	}		
	return $classTree;			
}

/*
 * Create an array of classes. Sort the array, then
 * create a 2-D array grouped by class name with each
 * class having an array of its help field data.
 */
function getHelpFromDb($classes)
{
	$fieldHelpItems = array();
	$classItems = array();
	
	foreach($classes as $className)
	{
		$class = new $className();
		$item = new fieldHelp($className, $class->prettifyClassName());
		array_push($classItems, $item);
	}

	$classItems = sortItems($classItems, "compareClasses");
	
	foreach($classItems as $classItem)
	{
		$class = new $classItem->class_name();
		$fields = array_keys($class->fields);
		if(is_array($class->additionalFields))
			$fields = array_merge($fields, $class->additionalFields);

		foreach($fields as $field)
		{
			$item = new fieldHelp($classItem->class_name, $classItem->pretty_class_name, $field);
			if(!is_array($fieldHelpItems[$classItem->class_name]))
				$fieldHelpItems[$classItem->class_name] = array();
			array_push($fieldHelpItems[$classItem->class_name], $item);
		}
	}
	
	$helps = query(ContextHelp);
	
	foreach($helps as $help)
	{
		if(array_key_exists($help->class_name, $fieldHelpItems))
		{
			$items = $fieldHelpItems[$help->class_name];
			foreach($items as $item)
			{
				if($help->field == $item->field)
					$item->help_key = $help->help_key;
			}
		}
	}

	return $fieldHelpItems;
}


class fieldHelp
{
	var $class_name;
	var $field;
	var $pretty_class_name;
	var $help_key;
	
	function fieldHelp($className, $pretty_class_name = "", $field = "")
	{
		$this->class_name = $className;
		$this->field = $field;
		/*if($pretty_class_name)
			$this->pretty_class_name = $pretty_class_name;
		else
			$this->pretty_class_name = $className;*/
		$this->pretty_class_name = prettify($className);
	}
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
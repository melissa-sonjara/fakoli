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
 * Given a class name, list the relations and field names. Allow
 * the user to select one. When selected, load the map into the
 * merge code form.
 * 
 */

Fakoli::using("email");
Fakoli::usingFeature("tree");

$class_name = mysql_escape_string($_GET["class_name"]);

if(!$class_name)
	die("");

$classes = ComponentManager::fireEvent("EnumerateDataItemClasses", $classes);

if(!array_search($class_name, $classes))
	Fakoli::end("No relations available for this class");

$obj = new $class_name();
$relations = $obj->relations;

$tree = new TreeControl("relation", "", "tree", true, 375);
$tree->height = "200";
$tree->indent = "0px";
$tree->selectMode = 'single';

if(count($relations) > 0)
{
	foreach($relations as $name => $class_name)
	{
		if(preg_match("/s$/", $name))
			continue;
		if(array_search($class_name, $classes))
		{	
			$relationNode = new TreeNode("relation_$name", $name, null, false, "bare_node_closed", "bare_node_open");
			$relObj = new $class_name();
			$fields = $relObj->fields;
			if(count($fields) > 0)
			{	
				foreach($fields as $f_name => $type)
				{
					if(preg_match("/_id/", $f_name) || preg_match("/composite/", $f_name) 
					|| preg_match("/password/", $f_name) || $type == Boolean)
					continue;
					
					$str = "'" . $f_name . "'";
					$fieldNode = new TreeNode("field_{$name}_{$f_name}", $f_name, null, false, "tree_node_closed", "tree_node_open", "javascript:new EmailManager().createMergeCode(\"$name\", \"$f_name\")");
	   				$fieldNode->leafStyle = "plain_node_leaf";
					$relationNode->add($fieldNode);
				}
			}
			$tree->add($relationNode);	
		}
	}
}

$script .= $tree->writeScript();

$tree->writeHTML();

?>
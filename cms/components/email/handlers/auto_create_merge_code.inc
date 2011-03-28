<?php
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
	ajaxReturn("<p>No relations available for this class");

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
	   				$fieldNode->leafStyle = "file_node_leaf";
					$relationNode->add($fieldNode);
				}
			}
			$tree->add($relationNode);	
		}
	}
}



$script .= $tree->writeScript();

$script .= <<<ENDSCRIPT
<script type="text/javascript">

function createMergeCode(class_name)
{
	alert(class_name);
}
</script>
ENDSCRIPT;
echo $script;

$tree->writeHTML();

?>
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

Fakoli::using("admin");
Fakoli::usingFeature("tree");


$src = $_GET["src"];
$page = $_GET["page"];

$components = Query::create(Component, "WHERE enabled=1 ORDER BY name")->execute();

$helpTree = new TreeControl("help_tree");
$helpTree->scroll = false;
$helpTree->width = 0;
$helpTree->style = "help_tree";

foreach($components as $component)
{
	$helpDir = $component->component_path . DIRECTORY_SEPARATOR . "help";
	
	if (!file_exists($helpDir)) continue;

	$helpMap = $helpDir . DIRECTORY_SEPARATOR . "help.inc";
	
	if (file_exists($helpMap))
	{
		include($helpMap);
		
		$title = array_key_exists("title", $help_map) ? $help_map['title'] : prettify($component->name);
		$node = new TreeNode("helppage_{$component->name}", $title, null, false, "tree_node_closed", "tree_node_open", "/action/admin/page?src={$component->name}&page={$help_map['default']}", "Two");
		$node->leafStyle = "file_node_leaf";
		
		foreach($help_map['pages'] as $identifier => $title)
		{
			$child = new TreeNode("helppage_{$component->name}_{$identifier}", $title, null, false, "tree_node_closed", "tree_node_open", "/action/admin/page?src={$component->name}&page={$identifier}", "Two");
			$child->leafStyle = "file_node_leaf";
			$node->add($child);
			
			if ($page == $identifier && $src == $component->name)
			{
				$child->checked = true;
				$node->open = true;
			}
		}
		
		$helpTree->add($node);
	}
	else
	{
		// Default help layout
		$node = new TreeNode("helppage_{$component->name}", prettify($component->name), null, false, "tree_node_closed", "tree_node_open", "/action/admin/page?src={$component->name}&page=", "Two");
		$node->leafStyle = "file_node_leaf";
		
		$files = scandir($helpDir);
		foreach($files as $file)
		{
			if (endsWith($file, ".inc"))
			{
				$identifier = substr($file, 0, -4);
				
				$child = new TreeNode("helppage_{$component->name}_{$identifier}", prettify($identifier), null, false, "tree_node_closed", "tree_node_open", "/action/admin/page?src={$component->name}&page={$identifier}", "Two");
				$child->leafStyle = "file_node_leaf";
				$node->add($child);
			}

			if ($page == $identifier && $src == $component->name)
			{
				$child->checked = true;
				$node->open = true;
			}
		}
	
		$helpTree->add($node);
	}
}

$script = $helpTree->writeScript();

AdminManager::startFrame();

$helpTree->writeHTML();

AdminManager::endFrame();
?>
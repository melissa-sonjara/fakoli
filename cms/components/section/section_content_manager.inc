<?php
/**\addtogroup section
 * @{
 */


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

/**
 * Defines the interface required by a SectionContentManager.
 * @author andy
 */
interface ISectionContentManager
{
	/**
	 * Returns the identifier for the default page in the section (i.e. the page that 
	 * should be served when no identifier is specified).
	 */
	function getDefaultPage($section);
	
	/**
	 * Returns a SectionContent object for the specified section and identifier
	 * @param Section $section
	 * @param string $identifier
	 */
	function getContent($section, $identifier);
	
	/**
	 * Renders and sends the specified content for the given section
	 * @param Section $section
	 * @param string $identifier
	 */
	function sendContent($section, $identifier);
	
	/**
	 * Factory method to build the view class for displaying and manipulating
	 * section content for the specified section.
	 * @param unknown $section
	 */
	function getAdminView($section);
}

class SectionContentManager implements ISectionContentManager
{
	function getDefaultPage($section) { return $section->default_page; }
	
	function getContent($section, $identifier)
	{
		try
		{
			return Query::create('SectionContent', "WHERE section_id=:section_id AND identifier=:identifier")
						->bind(":section_id", $section->section_id, ":identifier", $identifier)
						->executeSingle();
		}
		catch(DataNotFoundException $e)
		{
			return null;
		}
	}
	
	function sendContent($section, $identifier)
	{
		try
		{
			ComponentManager::fireEvent("ResolveIdentifier", $identifier, true);
		}
		catch(FakoliEventNotConsumed $e)
		{
			throw new FakoliException("The resource '$identifier' is disabled");
		}
		
	}
	
	function getAdminView($section)
	{
		return new SectionContentAdminView($section);
	}
}

abstract class AbstractSectionContentAdminView
{
	protected $section;
	
	abstract function handlePOST();
	abstract function drawView();
	
	function __construct($section)
	{
		$this->section = $section;
	}
}

class SectionContentAdminView extends AbstractSectionContentAdminView
{
	function __construct($section)
	{
		parent::__construct($section);
	}
	
	function handlePOST()
	{
		$doomed = new SectionContent();
		checkNumeric($_POST["content"]);
		
		if (isset($_POST["content"]))
		{
			foreach($_POST["content"] as $identifier => $id)
			{
				$doomed->delete("WHERE section_content_id=$id");
			}
		}
	
		if (isset($_POST["available"]))
		{
			foreach($_POST["available"] as $identifier => $id)
			{
				$content = new SectionContent();
				$content->section_id = $this->section->section_id;
				$content->identifier = $identifier;
				$content->save();
			}
		}			
	}
	
	function drawView()
	{
		global $script;
		global $dialogs;
		
		$content = $this->section->Content("ORDER BY identifier");
		
		$contentByIdentifier = reindexList($content, "identifier");		
		
		$itemsByType = array();		
		$itemsByType = ComponentManager::fireEvent("EnumerateItems", $itemsByType);
		
		$types = $this->categorizeItemTypes($itemsByType);
		 
		$contentTree  = new TreeControl("section_content_tree", "Section Content");
		$contentTree->scroll = true;
		$contentTree->width = 360;
		$contentTree->height = 430;
		$contentTree->selectOnRightClick = true;
		
		$availableTree = new TreeControl("available_content_tree", "Available Content");
		$availableTree->scroll = true;
		$availableTree->width = 360;
		$availableTree->height = 430;

		$categories = array();
		
		foreach($itemsByType as $type => $items)
		{
			$category = new TreeNode("content_type_".codify($type), $type, null, false, "tree_node_closed", "tree_node_open");
		
			$categories[$type] = new TreeNode("category_".codify($type), $type, null, false, "tree_node_closed", "tree_node_open");
			
			foreach($items as $item)
			{
				if (array_key_exists($item->identifier, $contentByIdentifier)) continue;
				
				$child = new TreeNode("available[{$item->identifier}]", $item->identifier, $item->identifier, false, "plain_tree_closed", "plain_tree_open");
				$child->leafStyle = "file_node_leaf";
				
				$category->add($child);
			}
			
			$availableTree->add($category);
		}

		foreach($content as $item)
		{
			if (!array_key_exists($item->identifier, $types)) continue;
			
			$type = $types[$item->identifier];
			$properties = $contentByIdentifier[$item->identifier];
			
			$child = new TreeNode("content['{$item->identifier}']", $item->identifier, $item->section_content_id, false, "plain_tree_closed", "plain_tree_open");
			$child->leafStyle = ($properties->template || $properties->role || $properties->permissions || $properties->use_SSL) ? "flagged_node_leaf" : "file_node_leaf";
			$categories[$type]->add($child);
		}
		
		foreach($categories as $category)
		{
			$contentTree->add($category);
		}

		$script .= <<<ENDSCRIPT
	<script src="/fakoli/js/section_content_manager.js" type="text/javascript"></script>
	<script type="text/javascript">
	
	var sectionContentManager;
	
	window.addEvent("domready", function()
	{
		sectionContentManager = new SectionContentManager('section_content_tree_table');
	});
	
	</script>
ENDSCRIPT;

		$contextMenu = new ContextMenu("section_content_menu", "#section_content_tree_table");
		$contextMenu->command("set_template", "Set Template...", "sectionContentManager.setTemplate()", false)
					->command("set_roles",	"Set Roles...", "sectionContentManager.setRole()", false)
					->command("set_permissions", "Set Permissions...", "sectionContentManager.setPermissions()", false)
					->command("set_ssl", "Configure SSL...", "sectionContentManager.setSSL()", false)
					->command("set_page_title", "Override Page Title...", "sectionContentManager.setPageTitle()", false)
					->command("set_body_class", "Set Body Class...", "sectionContentManager.setBodyClass()", false)
					->command("set_all", "All Properties...", "sectionContentManager.allProperties()", "", false)
					->separator()
					->command("use_defaults", "Use Section Defaults", "sectionContentManager.clearProperties()", "", false)
					->separator()
					->command("position_modules", "Position Modules", "sectionContentManager.positionModules()", "", false);

		$script .= $contextMenu->writeScript();
		$dialogs .= $contextMenu->writeMenu();

	?>
<form method="POST" action="?section_id=<?echo $this->section->section_id?>">
<div style="float: left; width: 365px; margin-left: 10px">
<? $contentTree->writeHTML()?>
</div>
<div id="section_content_buttons">
<button class="button" id="section_move_left" title="Add"></button>
<button class="button" id="section_move_right" title="Remove"></button>
</div>
<div style="float: left; width: 365px">
<? $availableTree->writeHTML()?>
</div>
<div style="clear: left"><br/></div>
</form>
<?
	}

	function categorizeItemTypes($itemsByType)
	{
		$types = array();
		
		foreach($itemsByType as $type => $items)
		{
			foreach($items as $item)
			{
				$types[$item->identifier] = $type;
			}
		}
	
		return $types;
	}
}

/**@}*/?>

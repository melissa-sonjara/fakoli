<?php
/**************************************************************
 * 
 * Title: status_tree.inc
 * 
 * Description: Classes for status trees and status
 * blocks for item list pages.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/17/10
 * 
 ***************************************************************/

require_once  $config['homedir']."/framework/tree.inc";
require_once  $config['homedir']."/framework/functions.inc";

/*
 * Generic class for presenting items in status trees
 * 
 * $items: groupedQuery results of items to display in status trees
 * $statusCategories: array e.g., array(In Progress => 1, In Review => 2, etc)
 * $fragmentUrl: the url of the status block fragment, e.g., "/portfolio/students/student_work_fragment.php/";
 * 
 * The items dataitem class must have function getTitle to get the title to 
 * display in the status tree line
 * 
 * The noneText should be set in the calling script. For example, "There are not
 * Crop It! activities in your portfolio."
 */
class StatusTrees
{
	var $items;
	var $trees;
	var $statusCategories;
	var $emptyMessage = ""; // You do not have any...
	var $width = 500;
	var $statusBlockView; // class that inherits from StatusBlockView and displays the status block contents	
	function StatusTrees($items, $statusCategories, $statusBlockView)
	{
		$this->items = $items;
		$this->statusCategories = $statusCategories;
		$this->statusBlockView = $statusBlockView;
	}
	
	function buildStatusTrees()
	{
		if(count($this->items) == 0) return;
			
		$trees = array();
				
		foreach($this->statusCategories as $statusTitle => $status)
		{
			if (array_key_exists($status, $this->items))
			{
				$tree = new TreeControl("statusTree$status", "", "tree", false, $this->width);
				$tree->height = "";
				$tree->indent = "0px";

				$category = $this->items[$status];
				
				foreach($category as $item)
				{
					$pkName = $item->getPrimaryKey();
					
					$node = $this->getNode($item, $pkName);

					$statusBlockView = new $this->statusBlockView($item);
					
					$subNode = new TreeNode("block_{$item->$pkName}", 
											$statusBlockView->getViewHTML(), "",
											false);
											$subNode->leafStyle = "";
					$node->add($subNode);
					
					$tree->add($node);
				}
		
				$trees[$status] = $tree;
			}
		}
		return $trees;
	}
	
	/*
	 * Enables inheriting classes to override this function and set their own title
	 */
	function getNode($item, $pkName)
	{
		$node = new TreeNode("status_{$item->$pkName}", $item->getTitle(), 
			null, false, "tree_list_closed", "tree_list_open");
		$node->leafStyle = "tree_list_open";
		
		return $node;
	}
	
	
	function writeScript()
	{
		
	}
	
	
	function drawView()
	{
		$this->trees = $this->buildStatusTrees();
		echo "<br/>\n";

		if (count($this->items) > 0)
		{
			foreach($this->statusCategories as $statusTitle => $status)
			{
				if (array_key_exists($status, $this->items))
				{
					echo "<h3>$statusTitle</h3>\n";
					$this->trees[$status]->writeHTML();
				}			
			}
		}
		else
		{
			echo "<br/><em>$this->emptyMessage</em><br/>";
		}
		
	}
}


/*
 * StatusBlockView
 * 
 * Description: Handles display of assignment in the status fragments.
 * 
 */
abstract class StatusBlockView
{
	var $item;
	var $statusCategories;
	var $iconView;
	var $iconEdit;
	var $iconDelete;
	var $activityName;
	var $deleteUrl;
		
	function StatusBlockView($item)
	{
		$this->item = $item;
	}
	
	// formats the layout for inheriting class
	function drawView()
	{	
		// begin status block
		echo "<div class=\"status_block\">";

		echo "<div id=\"col2\"><br/>";
		
		$this->drawLinks();
		
		echo "</div>";
		
		$this->writeSummary();
		
		echo "</div>";
	}
	
	/* When status fragment is not loaded on demand */
	function getViewHTML()
	{
		global $user;
		
		$html = "<div class=\"status_block\">";

		$html .= "<div id=\"col2\" style=\"float: right\"><br/>\n";	
		$html .= $this->getLinksHTML();
		$html .= "</div>\n<br/>\n";
		$html .= $this->getSummaryHTML();
		$html .= "</div>\n";
		
		return $html;		
	}	

	function writeSummary()
	{
		// handled by inheriting class
	}
	
	function getSummaryHTML()
	{
		// handled by inheriting class
	}
	
	function drawLinks()
	{
		// handled by inheriting class - buttons linking to
		// submit, edit, view, etc.
	}
	
	function getLinksHTML()
	{
		// handled by inheriting class - buttons linking to
		// submit, edit, view, etc.
	}
	
	// generic button
	function writeButton($url, $icon, $text, $newWindow = false, $class = "")
	{
		echo $this->getButtonHTML($url, $icon, $text, $newWindow, $class);
	}
	
	function getButtonHTML($url, $icon, $text, $newWindow = false, $class = "")
	{
		if($newWindow)
			$target = "target='_blank'";
		
		$class = (!$class) ? "button" : $class;
		$img = ($icon) ? "<img src='$icon' style='border: none'>" : ""; 
		
		$html = "<p><a $target class=\"$class\" href=\"$url\">
		$img&nbsp;$text&nbsp;</a></p>";
		
		return $html;
	}
	
	function writeViewButton()
	{
		$this->writeButton($this->item->getViewUrl(), $this->iconView, "View this {$this->activityName}", false);
	}
	
	function writeEditButton()
	{
		$this->writeButton($this->item->getEditUrl(), $this->iconEdit, "Edit this {$this->activityName}", false);
	}
	
	function writeDeleteButton($class = "")
	{
		echo $this->getDeleteButtonHTML($class);
	}
	
	function writeViewPopupButton()
	{
		$url = $this->item->getViewUrl();
		
		echo "<a class=\"button\" href=\"$url\" onclick=\"popup('$url', '_blank', 750, 600); return false;\">
		<img src=\"$this->iconView\" alt=\"View\"/>&nbsp;View this $this->activityName&nbsp;</a><br/>";
	}
	

	function getDeleteButton($class = "")
	{
		$class = (!$class) ? "button" : $class;
		$icon = ($this->iconDelete) ? "<img src=\"$this->iconDelete\"/>\n" : "";

		$message = "Are you sure you want to delete this {$this->activityName}?";

		$html = "<p><a href=\"$this->deleteUrl\" class=\"$class\"   
		onclick=\"return confirm('$message');\">
		$icon&nbsp;Delete this $this->activityName</a></p>";
		
		return $html;
		
	}
	
	function getEditButton($class = "", $newWindow = false)
	{
		$html = $this->getButtonHTML($this->item->getEditUrl(), $this->iconEdit, "Edit this {$this->activityName}", $newWindow);
		
		return $html;
	}	
	
	function getViewButton($class = "")
	{
		$html = $this->getButtonHTML($this->item->getViewUrl(), $this->iconView, "View this {$this->activityName}", $newWindow);
		
		return $html;
		
	}
	
} // end class StatusBlockView


?>
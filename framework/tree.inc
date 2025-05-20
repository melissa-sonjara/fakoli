<?php
/**\addtogroup datadisplay
 * @{
 */
/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

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
 * This file provides a class-based API for generating
 * expanding tree controls.
 * 
 * 
 */
 
/**
 * Global flag to indicate whether the implement Javascript for the tree control
 * has been written yet for this page output.
 */
$_writtenTreeControlScript = false;

/**
 * Global reference to currently drawing tree instance.
 */
$_activeTree = null;


/**
 * TreeNode represents a node in the tree. Both leaf and branch nodes are represented
 * using instances of this class. If a node is to be a branch, child TreeNodes can be
 * added via the add() method.
 */
class TreeNode
{
	var $children = array();
	
	var $id;
	var $title;
	var $value;
	var $checked;
	var $closedStyle;
	var $openStyle;
	var $leafStyle;
	var $open;
	var $link;
	var $onDemand;
	var $disabled;
	var $target;
	var $extras;	///< Extra data/controls to be shown with this tree node.
	
	/**
	 * Create a new TreeNode.
	 * 
	 * @param string $id the ID for this node
	 * @param string $title the Title text for this node
	 * @param string $value the value for this node. If non-empty, a checkbox is generated when the node is displayed.
	 * @param boolean $checked flag indicating whether the node should display as selected
	 * @param string $closedStyle CSS class to use when the node is closed.
	 * @param string $openStyle CSS class to use when the node is open.
	 * @param string $link optional hyperlink URL for the node.
	 * @param string $target optional target to load the URL.
	 */
	function __construct($id, $title, $value = "", $checked = false, $closedStyle = "tree_node_closed", $openStyle = "tree_node_open", $link = null, $target="")
	{
		$this->id          = $id;
		$this->title       = $title;
		$this->value       = $value;
		$this->checked     = $checked;
		$this->closedStyle = $closedStyle;
		$this->openStyle   = $openStyle;
		$this->open        = false;
		$this->link		   = $link;
		$this->leafStyle   = "tree_node_leaf";
		$this->onDemand    = "";
		$this->disabled	   = false;
		$this->target      = $target;
		$this->extras	   = "";
	}
	
	/**
	 * Write the node out as HTML to the output buffer. 
	 * This method recursively descends into all child nodes.
	 */
	function writeHTML()
	{
		global $_activeTree;
		
		$c = count($this->children);

		if ($this->link)
		{
			$title = "<a href='{$this->link}' target='{$this->target}'>{$this->title}</a>{$this->extras}";
		}
		else
		{
			$title = "<a href='#' onclick='";
			
			if ($_activeTree->onSelect)
			{
				$title .= "{$_activeTree->onSelect}(\"{$this->value}\");";
			}
			else if ($_activeTree->selectMode != 'navigation')
			{
				$title .= "new DOMEvent(event).stop(); Tree.toggleCheckbox(this, \"{$_activeTree->id}\", \"{$_activeTree->selectMode}\")";
			}
			$title .= "; return false'";
			
			if ($_activeTree->selectOnRightClick)
			{
				$title .= " oncontextmenu='Tree.selectCheckbox(this, \"{$_activeTree->id}\", \"{$_activeTree->selectOnRightClickMode}\");'";
			}
			
			$title .= ">{$this->title}</a>{$this->extras}";
		}
		
		if ($c == 0 && !$this->onDemand)
		{
			// Leaf node
			echo "<div id='{$this->id}' class='{$this->leafStyle}'>";

			if (isset($this->value) && $this->value !== "" && $_activeTree->selectMode != 'navigation')
			{
?>
			<input type="checkbox" class="checkbox" name="<? echo $this->id?>" value="<? echo $this->value?>"<? 
				if ($this->checked) echo " checked";
				if ($this->disabled) echo " disabled";
				if ($_activeTree->selectMode == "single") 
				{
					echo " onclick=\"Tree.clearCheckBoxes('{$_activeTree->id}', this);";
					if ($_activeTree->onSelect)
					{
						echo " {$_activeTree->onSelect}('{$this->value}');";
					}
					echo "\"";
				}
				else if ($_activeTree->onSelect)
				{
					echo " onclick=\"{$_activeTree->onSelect}('{$this->value}');\"";
				}
				 ?>/>
<?
			}
		
			echo "$title</div>";
		}
		else
		{
			if ($this->open)
			{
				$style = $this->openStyle;
				$display = "block";
			}
			else
			{
				$style = $this->closedStyle;
				$display = "none";
			}
						
			if ($this->onDemand)
			{
				$cmd = "Tree.loadOnDemand('{$this->id}', '{$this->onDemand}');";
				
				if ($this->open)
				{
?>
<script type="text/javascript">
	window.addEvent('domready', function() {Tree.loadOnDemand('<?echo $this->id?>', '<?echo $this->onDemand?>');});
</script>
<?
				}					
			}
			else
			{
				$cmd = "";
			}
			
			$cmd .= "Tree.toggleFolder('{$this->id}', '{$this->openStyle}', '{$this->closedStyle}');";
?>
		<div id='<?echo $this->id?>' class='<?echo $style?>' onclick="<?echo $cmd ?>">
<?
			if (isset($this->value) && $this->value !== "" && $_activeTree->selectMode != 'navigation')
			{
?>
			<input type="checkbox" class="checkbox" name="<? echo $this->id?>" value="<? echo $this->value?>" <? 
				if ($this->checked) echo " checked";
				if ($this->disabled) echo " disabled";
				if ($_activeTree->selectMode == "single") 
				{
					echo " onclick=\"Tree.clearCheckBoxes('{$_activeTree->id}', this);";
					if ($_activeTree->onSelect)
					{
						echo " {$_activeTree->onSelect}('{$this->value}');";
					}
					echo "\"";
				}
				else if ($_activeTree->onSelect)
				{
					echo " onclick=\"{$_activeTree->onSelect}('{$this->value}');\"";
				}
				 ?>/>
<?
			}
?>
		<?echo $title?></div>
		<div id='<?echo $this->id?>_contents' style='padding-left: <?echo $_activeTree->indent ?>; display: <? echo $display?>'>
			
<?			
			for($i = 0; $i < $c; ++$i)
			{
				$this->children[$i]->writeHTML();
			}
?>
		</div>
<?
		}
	}
	
	/**
	 * Add a child TreeNode (or an array of children)
	 * 
	 * @param mixed $n the child or children to add
	 */
	function add($n)
	{
		if (is_array($n))
		{
			for($i = 0; $i < count($n); ++$i)
			{
				$this->children[] = $n[$i];
			}
		}
		else
		{
			$this->children[] = $n;
		}
	}
	
	/**
	 * Recursively determine whether the branch of the tree of 
	 * which this node is the root should be displayed as open, 
	 * based on the selection state of the node and its children.
	 */
	function calculateOpenState()
	{
		if ($this->open) return true;
		if ($this->checked)
		{
			$this->open = true;
			return true;
		}
		
		$c = count($this->children);
		
		for($i = 0; $i < $c; ++$i)
		{
			if ($this->children[$i]->calculateOpenState())
			{
				$this->open = true;
				return true;
			}
		}
		
		$this->open = false;
		return false;
	}
	
	function selectByValue($value)
	{
		if ($value === '')
		{
			$this->checked = false;
		}
		else if ($this->value == $value)
		{
			$this->checked = true;
		}
		
		$c = count($this->children);
		
		for($i = 0; $i < $c; ++$i)
		{
			$this->children[$i]->selectByValue($value);
		}
	}
}

/**
 * The TreeControl provides a class that can be used to generate 
 * dynamic tree controls for use in PHP forms (or pages).
 * 
 * 
 */
class TreeControl
{
	var $id;
	var $title;
	var $style;
	var $scroll;
	var $width;
	var $height;
	var $selectMode = "multi";
	var $value;
	var $indent = "16px";
	var $children;
	var $selectOnRightClick = false;
	var $selectOnRightClickMode = "context";
	
	/**
	 * Create a new TreeControl.
	 * 
	 * @param string $id the ID for the control
	 * @param string $title the title text for the control
	 * @param string $style the base CSS class for the tree
	 * @param boolean $scroll flag indicating whether the control should display
	 *                        scrollbars when the contents overflow a fixed width or height.
	 * @param integer $width width of the control in pixels
	 * @param integer $height height of the control in pixels
	 */
	function __construct($id, $title = "", $style="tree", $scroll=true,$width=500,$height=400)
	{
		$this->id = $id;
		$this->title = $title;
		$this->style = $style;
		$this->scroll = $scroll;
		$this->width = $width;
		$this->height = $height;
	}
	
	/**
	 * @deprecated include the /fakoli/js/tree.js support file instead.
	 * Writes the JavaScript support functions for the tree.
	 */
	function writeScript()
	{
	}
	
	/**
	 * Outputs the TreeControl as HTML to the current output buffer.
	 */
	function writeHTML()
	{
		global $_activeTree;
		
		$_activeTree= $this;
		
		$this->writeScript();
		$width = is_numeric($this->width) ? "{$this->width}px" : $this->width;
		$height = is_numeric($this->height) ? "{$this->height}px" : $this->height;
		
		if ($this->selectMode == "single")
		{
?>
<input type='hidden' name='<?echo $this->id ?>' id='<?echo $this->id?>' value='<?echo $this->value ?>'/>
<? 
		}
?><table id="<?echo $this->id?>_table" class="<? echo $this->style?>" cellpadding="0" cellspacing="0" <? if ($this->width) echo "style='width:{$width}'"; ?>>
		 <? 
		 if ($this->title)
		 {
		 ?>		 
		 <tr>
		  <th><? echo $this->title?></th>
		 </tr>
		 <?
		 }
		 
		 $w = $width ? $width : "100%";
		 ?>
		 <tr>
		  <td>
		   <div style="padding: 0; margin: 0; width: <? echo $w?>;<? if ($this->height > 0) { ?> height: <?echo $this->height?>px;<? } ?><?if ($this->scroll) echo "overflow: auto"?>">
<?
		$this->writeNodes();
?>
		   </div>
		  </td>
		 </tr>
		</table>
<?
		$_activeTree = null;
	}

	function setValue($value)
	{
		$this->value = $value;
		$c = count($this->children);
		
		for($i = 0; $i < $c; ++$i)
		{
			$this->children[$i]->selectByValue($value);
		}
	}
	
	function writeNodes()
	{
		$c = count($this->children);
		
		for($i = 0; $i < $c; ++$i)
		{
			$this->children[$i]->calculateOpenState();
			$this->children[$i]->writeHTML();
		}	
	}
	
	/**
	 * Adds a child TreeNode (or an array of children)
	 * 
	 * @param mixed $n the child or children to add
	 */
	function add($n)
	{
		if (is_array($n))
		{
			for($i = 0; $i < count($n); ++$i)
			{
				$this->children[] = $n[$i];
			}
		}
		else
		{
			$this->children[] = $n;
		}
	}
}

/**@}*/?>

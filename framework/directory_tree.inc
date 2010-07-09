<?php
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

$_writtenDirectoryTreeControlScript = false;

function buildFolderTree(&$tree, $folder, $base)
{
	$dir = opendir($folder);
	
	$root = _buildFolderTreeAux($folder, $base);
	$tree->add($root);
} 

function _buildFolderTreeAux($folder, $base)
{
	$dir = opendir("$base/$folder");

	$id = str_replace("/", "_", $folder);
	$node = new FolderNode($id, basename($folder), $folder);
	while( $directory = readdir($dir))
	{
		if ($directory == "." || $directory == "..") continue;
		
		if (is_dir("$base/$folder/$directory"))
		{
			
			$node->add(_buildFolderTreeAux("$folder/$directory", $base));
			
		}
	}
	
	closedir($dir);
	
	return $node;
}

class FileNode
{
	var $id;
	var $title;
	var $value;
	
	function FileNode($id, $title, $value)
	{
		$this->id = $id;
		$this->title = $title;
		$this->value = $value;
	}
	
		
	function calculateOpenState()
	{
		return false;
	}
		
	function writeHTML()
	{
		$icon_image = $this->_tree->getIcon($this->title);
		$class = ($this->value == $this->_tree->selectedValue) ? "selected" : "file";
?>
		<div><img src="<?echo $this->_tree->images['toggle_blank']?>" align="absmiddle"><img src="<?echo $icon_image?>" align="absmiddle"><a id="<?echo $this->id?>_link" class="<?echo $class?>" href="javascript:selectItem('<?echo $this->_tree->id?>', '<?echo $this->id?>', '<?echo $this->value?>')" ondblclick="<?echo $this->_tree->id?>_onDoubleClick()"><?echo $this->title?></a></div>
<?
	}
}


class FolderNode
{
	var $children = array();
	
	var $id;
	var $title;
	var $value;
	var $checked;
	var $closedStyle;
	var $openStyle;
	var $open;
	
	function FolderNode($id, $title, $value = "", $checked = false, $closedStyle = "tree_node_closed", $openStyle = "tree_node_open", $leafStyle = "tree_node_leaf")
	{
		$this->id          = $id;
		$this->title       = $title;
		$this->value       = $value;
		$this->checked     = $checked;
		$this->closedStyle = $closedStyle;
		$this->openStyle   = $openStyle;
		$this->leafStyle   = $leafStyle;
		$this->open        = false;
	}
	
	function writeHTML()
	{
		ksort($this->children);
		
		$c = count($this->children);

		if ($c == 0)
		{
			echo "<div class=\"{$this->leafStyle}\"><img src=\"{$this->_tree->images['toggle_blank']}\" align=\"absmiddle\"><img src=\"{$this->_tree->images['folder_closed']}\" align=\"absbottom\">";
			// Leaf node
			if ($this->value)
			{
				if ($this->_tree->selectMode == "folder")
				{
					if ($this->_tree->multiSelect)
					{
?>
			<input type="checkbox" class="checkbox" name="<? echo $this->id?>" value="<? echo $this->value?>"<? if ($this->checked) echo " checked";?>> <?echo $this->title?>
<?
					}
					else
					{
						$class = ($this->value == $this->_tree->selectedValue) ? "selected" : "folder";
?>
			<a id="<?echo $this->id?>_link" class="<? echo $class?>" href="javascript:selectItem('<?echo $this->_tree->id?>', '<?echo $this->id?>', '<?echo $this->value?>')"><?echo $this->title?></a>
<?
					}
				}
				else
				{
?>
					<a id="<?echo $this->id?>_link" class="folder" href="" onclick="return false;"><?echo $this->title?></a>
<?
				}
				
			}
		
			echo "</div>";
		}
		else
		{
			if ($this->open)
			{
				$toggle = $this->_tree->images['toggle_open'];
				$folder = $this->_tree->images['folder_open'];
				$display = "block";
			}
			else
			{
				$toggle = $this->_tree->images['toggle_closed'];
				$folder = $this->_tree->images['folder_closed'];			
				$display = "none";
			}
?>
		<div id='<?echo $this->id?>'><img id="<?echo $this->id?>_toggle" src="<?echo $toggle?>" align="absmiddle" onclick="toggleDirectoryFolder('<?echo $this->id?>')"><img id="<?echo $this->id?>_folder" align="absmiddle" src="<?echo $folder?>" onclick="toggleDirectoryFolder('<?echo $this->id?>')">
<?
			if ($this->value)
			{
				if ($this->_tree->selectMode == "folder")
				{
					if ($this->_tree->multiSelect)
					{
?>
				<input type="checkbox" class="checkbox" name="<? echo $this->id?>" value="<? echo $this->value?>"<? if ($this->checked) echo " checked";?>> <?echo $this->title?>
<?
					}
					else
					{
						$class = ($this->value == $this->_tree->selectedValue) ? "selected" : "folder";
?>
				<a id="<?echo $this->id?>_link" class="folder" href="javascript:selectItem('<?echo $this->_tree->id?>', '<?echo $this->id?>', '<?echo $this->value?>')"><?echo $this->title?></a>
<?
					}
				}
				else
				{
?>
					<a id="<?echo $this->id?>_link" class="folder" href="javascript:toggleDirectoryFolder('<?echo $this->id?>')"><?echo $this->title?></a>
<?
				}			}
?>
		</div>
		<div id='<?echo $this->id?>_contents' style='padding-left: 16px; display: <? echo $display?>'>
			
<?			
			foreach($this->children as $key => $child)
			{
				$child->_tree =& $this->_tree;
				$child->writeHTML();
			}
?>
		</div>
<?
		}
	}
	
	function add(&$n)
	{
		trace($this->id." adding ".$n->id, 3);
		if (is_array($n))
		{
			for($i = 0; $i < count($n); ++$i)
			{
				$this->children[$n[$i]->title] =& $n[$i];
			}
		}
		else
		{
			$this->children[$n->title] =& $n;
		}
	}
	
	function calculateOpenState()
	{
		if ($this->open) return true;
		if ($this->checked)
		{
			$this->open = true;
			return true;
		}
		
		$c = count($this->children);
		
		foreach($this->children as $key => $child)
		{
			if ($child->calculateOpenState())
			{
				$this->open = true;
				return true;
			}
		}
		
		$this->open = false;
		return false;
	}
}


class DirectoryTreeControl
{
	var $id;
	var $title;
	var $style;
	var $scroll;
	var $width;
	var $height;
	var $showFiles = true;
	var $selectedValue;
	var $multiSelect = false;
	var $selectMode = "folder";
	
	var $children;
	var $images = array("toggle_open"	=>	"/images/toggle_open.png",
						"toggle_closed"	=>	"/images/toggle_closed.png",
						"toggle_blank"  =>	"/images/toggle_blank.png",
						"folder_open"	=>	"/images/folder_open.png",
						"folder_closed"	=>	"/images/folder_closed.png",
						"generic_file"	=>	"/images/file.png");
	
	var $icons = array(	".doc"		=>	"/images/msword_icon.png",
						".xls"		=>	"/images/msexcel_icon.png",
						".pdf"		=>	"/images/pdf_icon.png",
						".ppt"		=>	"/images/ppt_icon.png",
						"default"	=>	"/images/file_icon.png");
						
	
	function DirectoryTreeControl($id, $title = "", $showFiles = true, $style="tree", $scroll=true,$width=500,$height=400)
	{
		$this->id = $id;
		$this->title = $title;
		$this->style = $style;
		$this->scroll = $scroll;
		$this->width = $width;
		$this->height = $height;
		$this->showFiles = $showFiles;
	}
	
	function writeScript()
	{
		global $_writtenDirectoryTreeControlScript;
		
		if (!$_writtenDirectoryTreeControlScript)
		{
?>
<script type="text/javascript">
<!--
function toggleDirectoryFolder(id, openStyle, closedStyle)
{
	var div = document.getElementById(id + "_contents");
	var link = document.getElementById(id);
	var toggle = document.getElementById(id + "_toggle");
	var folder = document.getElementById(id + "_folder");

	if (div.style.display == "none" || div.style.display == "")
	{
		div.style.display = "block";
		link.className = openStyle;
		toggle.src = "<?echo $this->images['toggle_open']?>";
		folder.src = "<?echo $this->images['folder_open']?>";
	}
	else
	{
		div.style.display="none";
		link.className = closedStyle;
		toggle.src = "<?echo $this->images['toggle_closed']?>";
		folder.src = "<?echo $this->images['folder_closed']?>";
	}
}

function selectItem(treeID, itemID, itemValue)
{
	_clearSelectionImpl(treeID);
	var hidden = document.forms[0][treeID + "_value"];
	hidden.value = itemValue;
	
	var item = document.getElementById(itemID + "_link");
	item.className = "selected";
	eval(treeID + "_onSelectItem('" + itemID + "', '" + itemValue + "');");
}

function clearSelection(treeID)
{

	_clearSelectionImpl(treeID);
	eval(treeID + "_onClearSelection();");
}

function _clearSelectionImpl(treeID)
{
	var hidden = document.forms[0][treeID + "_value"];
	hidden.value = "";
	var tree = document.getElementById(treeID);
	var elts = tree.getElementsByTagName("a");
	for (i = 0; i < elts.length; ++i)
	{
		if (elts[i].className == "selected")
		{
			elts[i].className = "<?echo $this->selectMode?>";
			
		}
	}
}

//-->
</script>
<?
			$_writtenTreeControlScript = true;
		}
	
	// Event Hooks
	
?>
<script type="text/javascript">
function <?echo $this->id?>_onSelectItem(itemID, itemValue)
{
<? 
	if ($this->onSelectionChanged)
	{
?>
		<?echo $this->onSelectionChanged?>(itemID, itemValue);
<?
	}
?>
}

function <?echo $this->id?>_onClearSelection()
{
<? 
	if ($this->onSelectionChanged)
	{
?>
		<? echo $this->onSelectionChanged?>(null, null);
<?
	}
?>
}

function <?echo $this->id?>_onDoubleClick(itemID, itemValue)
{
<?
	if ($this->onDoubleClick)
	{
?>
		<? echo $this->onDoubleClick?>(itemID, itemValue);
<?
	}
?>
}

</script>
<?
	}
	
	function writeHTML()
	{
		trace("DirectoryTreeControl::writeHTML()", 3);
		
		ksort($this->children);
		$this->writeScript();
?>
		<input type="hidden" name="<?echo $this->id?>_value" value="<?echo $this->selectedValue?>">
		<table id="<?echo $this->id?>" class="<? echo $this->style?>" cellpadding="0" cellspacing="0">
		 <? 
		 if ($this->title)
		 {
		 ?>		 
		 <tr>
		  <th><? echo $this->title?></th>
		 </tr>
		 <?
		 }
		 ?>
		 <tr>
		  <td>
		   <div style="width: <? echo $this->width?>px;height: <?echo $this->height?>px;<?if ($this->scroll) echo "overflow: auto"?>">
<?
		$c = count($this->children);
		foreach($this->children as $key => $child)
		{
			trace("Writing $key,{$child->title}", 3);
			$child->_tree =& $this;
			$child->calculateOpenState();
			$child->writeHTML();
		}
?>
		   </div>
		  </td>
		 </tr>
		</table>
<?
	}

	function getIcon($file)
	{
		$ext = substr($file, strrpos($file, "."));
		$icon = $this->icons[$ext];
		trace("getIcon($file) $ext $icon", 3);
		
		if (!isset($icon))
		{
			$icon = $this->icons["default"];
		}
		
		return $icon;			
	}
	
	function add(&$n)
	{
		trace($this->id." adding ".$n->id, 3);
		if (is_array($n))
		{
			for($i = 0; $i < count($n); ++$i)
			{
				$this->children[$n[$i]->title] = &$n[$i];
			}
		}
		else
		{
			$this->children[$n->title] =& $n;
		}
	}
	
	function &findNode($path)
	{
		trace("DirectoryTree:findNode($path)", 3);
		$elts = split("/", $path);
		
		$node =& $this;
		
		foreach($elts as $elt)
		{
			$match = false;
			
			if (isset($node->children[$elt]))
			{
				$node =& $node->children[$elt];
			}
			else return null;
		}
		
		if ($node === $this) $node = null;
		
		trace("Found Node: {$node->id}", 3);
		return $node;
	}
}

?>
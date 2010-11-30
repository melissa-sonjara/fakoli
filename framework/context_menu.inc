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

class ContextMenuItem
{
	var $command_id;
	var $icon;
	var $text;
	var $action;

	function ContextMenuItem($command_id, $text, $action, $icon = "")
	{
		$this->command_id = $command_id;
		$this->icon = $icon;
		$this->text = $text;
		$this->action = $action;
	}
	
	function draw()
	{
		if ($this->icon)
		{
			$output = "<img src='{$this->icon}' alt='' style='float: left; margin: 2px 0px 0px 1px'/>";
		}

		$output .= "<a href='{$this->action}'>";
		
		$output .= $this->text;
		$output .= "</a>";
		echo $output;
	}
}

class ContextMenuSeparator
{
	function ContextMenuSeparator()
	{
	}
	
	function draw()
	{
		echo "<hr/>";
	}
}

class ContextMenu
{
	var $id;
	var $cssClass;
	var $items = array();
	var $targets;
	var $trigger = "contextmenu";
	
	function ContextMenu($id, $targets, $cssClass = "context_menu", $trigger = "contextmenu")
	{
		$this->id = $id;
		$this->targets = $targets;
		$this->cssClass = "context_menu";
		$this->trigger = $trigger;
	}
	
	function command($id, $text, $action, $icon = "")
	{
		$this->items[] = new ContextMenuItem($id, $text, $action, $icon);
		return $this;
	}
	
	function separator()
	{
		$this->items[] = new ContextMenuSeparator();
		return $this;
	}
	
	function writeScript()
	{
		$s = <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function() {
new ContextMenu('$this->id', '$this->targets', '$this->trigger');
});
</script>
ENDSCRIPT;

		return $s;
	}

	function writeMenu()
	{
		ob_start();
?>
	 <ul id="<?echo $this->id?>" class="<?echo $this->cssClass?>">
<?
		foreach($this->items as $item)
		{
			echo "<li>";
			$item->draw();
			echo "</li>";
		}	
?>
	 </ul>
<?
		$menu = ob_get_contents();
		ob_end_clean();
		return $menu;
	}
}
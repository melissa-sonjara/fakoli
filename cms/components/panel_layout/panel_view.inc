<?php

interface PanelControl
{
	function writeScript();
	function drawControl();
}
	

class PanelButton implements PanelControl
{
	var $icon;
	var $text;
	var $tooltip;
	var $action;
	
	function PanelButton($icon, $text, $tooltip, $action)
	{
		$this->icon = $icon;
		$this->text = $text;
		$this->tooltip = $tooltip;
		$this->action = $action;
	}

	function writeScript()
	{
	}
	
	function drawControl()
	{
?>
	<img src="<?echo $this->icon?>" alt="<?echo $this->text?>" title="<?echo $this->tooltip?>" style="border: none; display:inline-block;vertical-align: middle" onclick="<?echo $this->action?>; return false"/>
<?
	}
}


class PanelSeparator implements PanelControl
{
	var $width;
	
	function PanelSeparator($width)
	{
		$this->width = $width;
	}
	
	function writeScript()
	{
	}
	
	function drawControl()
	{
		echo "<div style='display: inline-block;width: {$this->width}px'>&nbsp;</div>";
	}
}

class PanelToolbar
{
	var $controls;

	function PanelToolbar()
	{
		$this->controls = array();		
	}
	
	function button($icon, $text, $tooltip, $action)
	{
		$this->controls[] = new PanelButton($icon, $text, $tooltip, $action);		
		return $this;
	}

	function separator($width = 8)
	{
		$this->controls[] = new PanelSeparator($width);
		return $this;
	}
	
	function control($control)
	{
		$this->controls[] = $control;
		return $this;
	}
	
	function writeScript()
	{
		foreach($this->controls as $control)
		{
			$control->writeScript();
		}
	}
	
	function drawToolbar()
	{
?>
<div class="panel_toolbar">
<?
		foreach($this->controls as $control)
		{
			$control->drawControl();
		}
?>
</div>
<?
	}
}

class PanelView
{
	var $id;
	var $title;
	
	var $content;
	var $dialogs;
	var $menu;
	var $script;
	var $buttons;
	var $controls;
	var $toolbars;
	
	function PanelView($id, $title)
	{
		$this->id = $id;
		$this->title = $title;
		
		$this->buttons = array();
		$this->controls = array();
	}
	
	function button($icon, $text, $tooltip, $action)
	{
		$this->buttons[] = new PanelButton($icon, $text, $tooltip, $action);		
		return $this;
	}
	
	function control($control)
	{
		$this->controls[] = $control;
		return $this;
	}
	
	function toolbar($toolbar)
	{
		$this->toolbars[] = $toolbar;
		return $this;
	}
	
	function writeScript()
	{
		if ($this->menu)
		{
			$this->script .= $this->menu->writeScript();
		}
		
		$this->script .= <<<ENDSCRIPT
<script type="text/javascript">
var {$this->id}_body = $('{$this->id}_body');
if ({$this->id}_body.panel)
{
	var panel = {$this->id}_body.panel;
	
	panel.body = {$this->id}_body;
	panel.header = $('{$this->id}_header');
}
</script>
ENDSCRIPT;

		if (count($this->controls) > 0)
		{
			foreach($this->controls as $control)
			{
				$this->script .= $control->writeScript();
			}
		}
		
		if (count($this->toolbars) > 0)
		{
			foreach($this->toolbars as $toolbar)
			{
				$this->script .= $toolbar->writeScript();
			}
		}
		
		if (is_object($this->title)) $script .= $this->title->writeScript();
		
		return $this->script;
	}
	
	function commandMenu()
	{
		$this->menu = new ContextMenu($this->id . "_menu", "#{$this->id}_menu_button");
		$this->menu->trigger = "click";
		
		return $this->menu;
	}
	
	function drawView()
	{
		if ($this->menu) $this->dialogs .= $this->menu->writeMenu();
		
		ob_start();
?>
		<?echo $this->writeScript()?>
		<?echo $this->dialogs?>
		 <div id="<?echo $this->id?>_header" class="panel_header">
<?
		if ($this->menu)
		{
?>
	  		<img id="<?echo $this->id?>_menu_button" src="/fakoli/images/data_view_menu.png" alt="Panel Menu" style="border: none; display:inline-block;vertical-align: middle"/>
<?		
		}

		if (is_object($this->title)) 
		{ 
			$this->title->drawControl(); 
		} 
		else 
		{ 
			echo $this->title;
		}
		
		if (count($this->buttons) > 0)
		{
?>
			<div id="<?echo $this->id?>_buttons" class="panel_buttons">
<?
			foreach($this->buttons as $button)
			{
				$button->drawControl();
			}
?>			
			</div>
<?
		}
		
		if (count($this->controls) > 0)
		{
			foreach($this->controls as $control)
			{
				$control->drawControl();
			}
		}
		
		if (count($this->toolbars) > 0)
		{
			foreach($this->toolbars as $toolbar)
			{
				$this->script .= $toolbar->writeScript();
			}
		}
?>
		 </div>
		 <div id="<?echo $this->id?>_body" class="panel_body">
		 <?echo $this->content?> 
		 </div>
<?
		$out = ob_get_contents();
		ob_end_clean();
		return $out;		
	}
}


?>
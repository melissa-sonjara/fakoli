<?php

class PanelButton
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
	
	function draw()
	{
?>
	<img src="<?echo $this->icon?>" alt="<?echo $this->text?>" title="<?echo $this->tooltip?>" style="border: none; display:inline-block;vertical-align: middle" onclick="<?echo $this->action?>; return false"/>
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
	
	function PanelView($id, $title)
	{
		$this->id = $id;
		$this->title = $title;
		
		$this->buttons = array();
	}
	
	function button($icon, $text, $tooltip, $action)
	{
		$this->buttons[] = new PanelButton($icon, $text, $tooltip, $action);		
		return $this;
	}
	
	function writeScript()
	{
		if ($this->menu)
		{
			$this->script .= $this->menu->writeScript();
		}
		
		$script .= <<<ENDSCRIPT
<script type="text/javascript">
var <?echo $this->id?>_body = $('<?echo $this->id?>_body');
if (<?echo $this->id?>_body.panel)
{
	panel.body = <?echo $this->id?>_body;
	panel.header = $('<?echo $this->id?>_header');
}
</script>
ENDSCRIPT;
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
?>	
		  <?echo $this->title?>
<?
		if (count($this->buttons) > 0)
		{
?>
			<div id="<?echo $this->id?>_buttons" class="panel_buttons">
<?
			foreach($this->buttons as $button)
			{
				$button->draw();
			}
?>			
			</div>
<?
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
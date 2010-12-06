<?php

class PanelView
{
	var $title;
	
	var $content;
	var $dialogs;
	var $menu;
	
	function PanelView($title)
	{
		$this->id = $id;
		$this->title = $title;
	}
	
	
	function writeScript()
	{
	}
	
	function drawView()
	{
		ob_start();
?>
		<?echo $this->writeScript()?>
		<?echo $this->dialogs?>
		 <div class="panel_header">
		  <?echo $this->title?>
		 </div>
		 <div class="panel_body">
		 <?echo $this->content?> 
		 </div>
<?
		$out = ob_get_contents();
		ob_end_clean();
		return $out;		
	}
}


?>
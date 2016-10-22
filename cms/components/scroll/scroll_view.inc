<?php 

class ScrollView
{
	var $scroll;
	
	function __construct($scroll)
	{
		$this->scroll = $scroll;	
	}

	function renderPart($part, $partTemplate)
	{
		global $script;
		
		if ($part->content_type == "Code")
		{
			ob_start();
			include $part->php_code_file;
			$part->content = ob_get_contents();
			ob_end_clean();
		}
		
		return $part->format($partTemplate);
	}
	
	function drawView()
	{
		$out = "";
		$partTemplate = Settings::getValue("scroll", "scroll_part_template");
		$parts = $this->scroll->Parts();
		foreach($parts as $part)
		{
			$out .= $this->renderPart($part, $partTemplate)."\n";
		}
		
		echo $out;
	}
}
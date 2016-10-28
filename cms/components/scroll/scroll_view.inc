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
			$part->content = $this->renderCodePart($part);
		}
		
		$out = $part->format($partTemplate);
		$out = preg_replace("/\\[((?:include|position):.*?)\\]/i", "{\$1}", $partTemplate);
		return $out;
	}
	
	function renderCodePart($part)
	{
		global $config;
		global $method;
		global $user;
		global $script;
		global $styles;
		global $breadcrumbs;
		global $dialogs;
		global $page;
		global $site;
		global $section;
		global $menu_identifier;
		global $identifier;
		global $section;
		
		trace("Rendering part '{$part->title}' with '{$part->php_code_file}'", 3);
		
		if ($part->php_code_file)
		{
			ob_start();
			
			try
			{
				$codePath = explode(",", $part->php_code_file);
				if (count($codePath) == 2)
				{
					$name = $codePath[0];
					$php_code_file = $codePath[1];
					$components = Fakoli::getComponents();
					if (array_key_exists($name, $components))
					{
						$path = $components[$name]->component_path;
						Fakoli::applicationTraceLevel();
						
						include $path."/{$php_code_file}";
	
						Fakoli::coreTraceLevel();
					}
					else
					{
						throw new FakoliException("Unknown component");
					}
				}
				else
				{
					Fakoli::applicationTraceLevel();
					
					include $config['homedir']."/{$part->php_code_file}";
					
					Fakoli::coreTraceLevel();
				}
	    		$output = trim(ob_get_contents());
			}
			catch(Exception $e)
			{
				$output = $e->__toString();
			}

			ob_end_clean();
		}	

		return $output;
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
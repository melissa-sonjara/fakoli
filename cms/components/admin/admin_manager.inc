<?php

class AdminManager
{
	static function startFrame($class)
	{
		global $script;
		$script = Fakoli::getScripts() . $script;
		$styles = Fakoli::getStyles();		
		$css = explode(",", Settings::getValue("online_help", "help_css"));
		
		foreach($css as $c)
		{
			$styles .= "<link href='$c' rel='stylesheet'/>\n";
		}
?>
<!DOCTYPE html>
<html>
 <head>
 <?echo $styles?>
 <?echo $script?>
 </head>
 <body class='<?echo $class?>'>
<?
	}
	
	static function endFrame()
	{
?>
 </body>
</html>
<?
	}
}
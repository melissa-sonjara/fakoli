<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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

Fakoli::using("settings");

class OnlineHelpSerializationHandler
{
	function OnlineHelpSerializationHandler()
	{
	}

	function export()
	{
		return SerializationManager::serialize(HelpBook, "ORDER BY help_book_id");
		return SerializationManager::serialize(HelpPage, "ORDER BY help_page_id");
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(HelpBook, $doc, $tx);
		SerializationManager::unserialize(HelpPage, $doc, $tx);
	}	
}

class OnlineHelpManager
{	
	var $book;
	
	function OnlineHelpManager($book)
	{
		$this->book = $book;
	}

	static function setDefaults()
	{
		Settings::setDefaultValue("online_help", "default_book", "", String, "Specify the default help book to which the help link will connect");
		Settings::setDefaultValue("online_help", "use_page_identifiers", "", Boolean, "Use page identifiers as the default help identifier");
		Settings::setDefaultValue("online_help", "help_text", "Help", String, "The text to display on the help link");
		Settings::setDefaultValue("online_help", "help_css", "/components/online_help/css/online_help.css", String, "CSS Files defining the help popup look and feel (comma separated)");	
	}
	
	static function onInitialize()
	{
		global $help_book;
		global $help_page;
	}
	
	static function substituteHelpLink($output)
	{
		global $help_book;
		global $help_page;
		global $page;
		
		$useIdentifier = Settings::getValue("online_help", "use_page_identifiers");
		if (!$help_book) $help_book = Settings::getValue("online_help", "default_book");
		if (!$help_page && $useIdentifier) $help_page = $page->identifier;
		
		if ($help_book && $help_page)
		{
			$helpText = Settings::getValue("online_help", "help_text");			
			$helpLink = "<a class='help_link' href='#' onclick='OnlineHelp.help(\"{$help_book}\", \"{$help_page}\"); return false;'>$helpText</a>";
			
			$output = str_replace("{help}", $helpLink, $output);
		}
		
		return $output;
	}
	
	static function getAdminTabs($key)
	{
		$tabs = array(	"Details"		=>	"/admin/help_book_form",
						"Help Pages"	=>	"/admin/help_pages"
						);
	
		$qs = ($key) ? "help_book_id=$key" : ""; 
		return new TabBar("tabs", $tabs, $qs);
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new OnlineHelpUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("online_help", "Online Help", new OnlineHelpSerializationHandler());
	}
	
	function startFrame($class)
	{
		global $script;
		$script = Fakoli::getScripts() . $script;
		$styles = Fakoli::getStyles();
		
		$css = explode(",", Settings::getValue("online_help", "help_css"));
		$additional = explode(",", $this->book->additional_css_files);
		
		foreach($css as $c)
		{
			$styles .= "<link href='$c' rel='stylesheet'/>\n";
		}
		
		foreach($additional as $c)
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
		$script = "";
	}
	
	function endFrame()
	{
?>
 </body>
</html>
<?
	}
}

?>
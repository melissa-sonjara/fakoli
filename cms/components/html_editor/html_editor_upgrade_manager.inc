<?
Fakoli::using("html_editor", "component");

class HtmlEditorUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Initial Installation",
		"1.1"	=>	"sql:Created snippet table",
		"1.2"	=>	"sql:Added description field to snippet table"

	);
	
	function __construct()
	{
		$this->component_name = "html_editor";
		
		parent::UpgradeManager();
	}
}

?>
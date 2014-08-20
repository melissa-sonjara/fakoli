<?php
/**
 * Manage schema updates for the tool_hints component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andrew Green
 */


Fakoli::using("tool_hints", "component");

class ToolHintsUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Create tool_hint and tool_hint_user_status tables",
		"1.1"	=>	"sql:Added can_be_hidden flag to tool_hint table",
		"1.2"	=>	"sql:Added read_access parameter to tool_hint table"
	);

	function ToolHintsUpgradeManager()
	{
		$this->component_name = "tool_hints";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
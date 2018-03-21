<?php
Fakoli::using("developer_tools");

$tables = Query::create(TableInformation, "WHERE TABLE_SCHEMA=DATABASE() AND TABLE_TYPE='BASE TABLE' ORDER BY TABLE_TYPE, TABLE_NAME")->execute();

$charset = new TableCharacterSet();

$form = new AutoForm($charset);

$characterSets = Query::create(CharacterSet, "ORDER BY CHARACTER_SET_NAME")->execute();
$options = array(""=>"");

foreach($characterSets as $set)
{
	$options[$set->CHARACTER_SET_NAME] = $set->format("{CHARACTER_SET_NAME} - {DESCRIPTION}");
}

$form->dropdown("character_set", "Convert to Character Set", $options);
$form->required("character_set");
$form->submitLabel = "Update Character Sets";

$tableSelect = new TableSelectFieldRenderer($form, "tables", "Tables",
	function($table)
	{
		$table->column("Table Name", "{TABLE_NAME}", true)
			  ->column("Collation", "{TABLE_COLLATION}", true)
			  ->column("# Rows", "{TABLE_ROWS}", true, "text-align: right");
		
		$table->sortable = true;
		$table->filter = true;
		$table->pageSize = 20;
		$table->emptyMessage = "Schema is empty";
		$table->excelFile = "table_list.xls";
		
		$table->showSelectAll = true;
		$table->commandMenu();
	},
	
	function($field)
	{
		if (!$field) $field = $this->field;
		$tables = $_POST[$field];
		$charset =  $_POST["character_set"];
		
		foreach($tables as $table)
		{
			trace("Updating character set for $table to $charset", 3);  
			TableCharacterSet::updateCharacterSet($table, $charset);
		}
		
		return true;
	},
	
	"multi", $tables
);
	
if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::notify("Changes Saved");
		$tableSelect->data = Query::create(TableInformation, "WHERE TABLE_SCHEMA=DATABASE() ORDER BY TABLE_TYPE, TABLE_NAME")->execute();
		
	}
	else
	{
		Fakoli::notify($form->msg);
	}
}

$script .= $form->writeScript();
$form->drawForm();
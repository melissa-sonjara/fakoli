<?php
class CharacterSet extends DataItem
{
	var $table = "information_schema.character_sets";
	var $primary_key = "CHARACTER_SET_NAME";
	
	var $fields = array("CHARACTER_SET_NAME"	=>	String,
						"DEFAULT_COLLATE_NAME"	=>	String,
						"DESCRIPTION"			=>	String,
						"MAXLEN"				=>	Number);
}

class TableCharacterSet extends DataItem
{
	var $primary_key = "table_name";
	
	var $fields = array("table_name"	=>	String,
						"character_set"	=>	String);
	
	function save()
	{
		return true;
	}
	
	static function updateCharacterSet($table, $charset)
	{
		checkIdentifier($table);
		checkIdentifier($charset);
		
		$stmt = "ALTER TABLE {$table} CHARACTER SET {$charset}";
		trace($stmt, 3);
		
		$db = ConnectionManager::getConnection();
		$db->exec($stmt);
	}
}
<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the data_sync component.
 */

Fakoli::using("client", "project", "biz_dev");

class DataSyncManager
{
	function DataSyncManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}


	static function getImportTabs()
	{
		$tabs = array(
				"Upload File"				=>	"data_import",
				"Select Target"				=>	"data_import_select_target",
				"Choose Fields"				=>	"data_import_field_mapping",
				"Select Records"			=>	"data_import_select"
		);
	
		$tabBar = new TabBar("tabs", $tabs);
		$tabBar->useQueryString = false;
		return $tabBar;
	
	}

	static function getAdminImportTabs()
	{
		$tabs = array(
				"Upload File"				=>	"/admin/data_import",
				"Select Target"				=>	"/admin/data_import_select_target",
				"Choose Fields"				=>	"/admin/data_import_field_mapping",
				"Select Records"			=>	"/admin/data_import_select"
		);
	
		$tabBar = new TabBar("tabs", $tabs);
		$tabBar->useQueryString = false;
		return $tabBar;
	
	}
	
	static function getImportColumns($file = "")
	{
		if (!$file)
		{
			$file = $_SESSION["data_import_file"];
		}
	
		$fp = fopen($file, "r");
	
		$fields = fgetcsv($fp);
	
		fclose($fp);
	
		$columns = array();
	
		foreach($fields as $field)
		{
			if ($field) $columns[] = $field;
		}
	
		return $columns;
	}

	static function getFieldMappings($class)
	{
		DataSyncManager::getSyncTargets();
		
		$fieldMappings = Query::create(DataImportFieldMapping, "WHERE class=:cl")
							  ->bind(":cl", $class)
							  ->execute();
	
		if (count($fieldMappings) == 0)
		{
			// Create default entries
				
			$obj = new $class();
			foreach($obj->fields as $field => $type)
			{
				if ($field == $obj->getPrimaryKey() || endsWith($field, "_id")) continue;
	
				$mapping = new DataImportFieldMapping();
				$mapping->class = $class;
				$mapping->client_field = $field;
				$mapping->import_column = "";
	
				$mapping->save();
			}
				
			$fieldMappings = Query::create(DataImportFieldMapping, "WHERE class=:cl")
							  ->bind(":cl", $class)
							  ->execute();
	
		}
	
		return $fieldMappings;
	}
	

	static function createImportItems($class, $file = "")
	{
		global $user;
		
		$matchingFields = Query::create(DataImportFieldMapping, "WHERE class=:cl and matching=1")
							   ->bind(":cl", $class)
							   ->execute();
		
		$mf = array();
		
		foreach($matchingFields as $matchingField)
		{
			 $mf[] = "{".$matchingField->client_field."}";
		}
		
		$matchingFormat = implode("##", $mf);
		
		$existingItems = IteratedQuery::create($class)->execute();
		
		$existingItemIDs = array();
		
		foreach($existingItems as $item)
		{
			$existingItemIDs[$item->format($matchingFormat)] = $item->get($item->getPrimaryKey());
		}
		
		$fieldMapping = IndexedQuery::create(DataImportFieldMapping, "WHERE class=:cl and import_column != ''", "client_field")
									->bind(":cl", $class)
									->execute();
	
		$columns = array();
	
		$items = array();
	
		if (!$file)
		{
			$file = $_SESSION["data_import_file"];
		}
	
		$fp = fopen($file, "r");
	
		$fields = fgetcsv($fp);
	
		$c = 0;
		foreach($fields as $field)
		{
			$columns[$field] = $c++;
		}
	
		while($record = fgetcsv($fp))
		{
			$item = new $class();
			$skip = false;
			
			$item->filter = new InclusionFilter();
			
			foreach($fieldMapping as $field => $mapping)
			{
				$item->filter->add($field);
				
				$cols = $mapping->import_column;
	
				if (!$cols) continue;
	
				$cols = explode(",", $cols);
	
				$vals = array();
	
				foreach($cols as $col)
				{
					if ($record[$columns[$col]])
					{
						$vals[] = $record[$columns[$col]];
					}
				}
	
				$separator = ($contact->fields[$field] == Text) ? "\n" : " ";
	
				$value = trim(implode($separator, $vals));
				
				if (!$value && $mapping->required)
				{
					$skip = true;
					break;
				}
				
				$item->set($field, $value);
			}
				
			if (!$skip) 
			{
				if ($item->hasField("owner_id")) $item->set("owner_id", $user->person_id);	
				
				if ($matchingFormat)
				{
					$mf = $item->format($matchingFormat);
					if (array_key_exists($mf, $existingItemIDs))
					{
						$item->set($item->getPrimaryKey(), $existingItemIDs[$mf]);
					}
				}
											
				$items[] = $item;
			}
		}
	
		fclose($fp);
	
		return $items;
	}
	
	static function onItemRow($item)
	{
		$val = $item->get($item->getPrimaryKey());
		return ($val) ? "matching" : "new";
	}
	
	static function isRowSelected($row)
	{
		return false;
	}
	
	static function getSyncTargets()
	{
		$targets = array();
		$targets = ComponentManager::fireEvent("RegisterSyncTargets", $targets);

		$syncTargets = array();
		foreach($targets as $component => $classes)
		{
			Fakoli::using($component);
			foreach($classes as $class)
			{
				$syncTargets[] = $class;
			}
		}
		
		return $syncTargets;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new DataSyncUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>
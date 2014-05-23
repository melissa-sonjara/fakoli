<?php

/**
 * IDataSyncHelper defines the interface that application should implement
 * when providing sync helpers to handle specific import logic.
 * @author andy
 */
interface IDataSyncHelper
{
	function getAdditionalColumns($columns);
	function processRecord($record);
	function extendForm($form);
}

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the data_sync component.
 */
 class DataSyncManager
{
	static $helpers = null;
	static $totalRecords = 0;
	
	function DataSyncManager()
	{
	}

	static function registerSyncHelper($class, $helper)
	{
		DataSyncManager::$helpers[$class] = $helper;
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("data_sync", "import_in_chunks", false, Boolean, "Specifies whether to limit the number of records being imported at one time.");
		Settings::setDefaultValue("data_sync", "records_per_page", 500, Number, "The number of records to import at one time when chunked import is enabled.");
		Settings::setDefaultValue("data_sync", "multiple_import_profiles", false, Boolean, "Turn on support for configuring multiple import profile for each sync target.");
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
	
	static function countRecords($file = "")
	{
		if (!$file)
		{
			$file = $_SESSION["data_import_file"];
		}
		
		$fp = fopen($file, "r");

		$counter = -1;
		while(fgetcsv($fp))
		{
			$counter++;
		}
		
		fclose($fp);
		
		return $counter;
	}
	
	static function skipToOffset($fp, $offset = -1)
	{
		if ($offset == -1)
		{
			$offset = checkNumeric($_GET["offset"]);
		}
		
		while($offset-- > 0)
		{
			fgetcsv($fp);
		}
	}
	
	static function getImportColumns($class, $file = "")
	{
		ComponentManager::fireEvent("RegisterSyncHelpers");
		
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
	
		if (array_key_exists($class, DataSyncManager::$helpers))
		{
			$helper = DataSyncManager::$helpers[$class];
			$columns = $helper->getAdditionalColumns($columns);
		}
		
		return $columns;
	}

	static function extendForm($form)
	{
		$class = get_class($form->data);
		if (array_key_exists($class, DataSyncManager::$helpers))
		{
			$helper = DataSyncManager::$helpers[$class];
			$helper->extendForm($form);
		}
	}
	
	static function getFieldMappings($class, $profile_id = 0)
	{
		DataSyncManager::getSyncTargets();
		
		$fieldMappings = Query::create(DataImportFieldMapping, "WHERE class=:cl AND import_profile_id=:pr")
							  ->bind(":cl", $class, ":pr", $profile_id)
							  ->execute();
	
		if (count($fieldMappings) == 0)
		{
			// Create default entries
				
			$obj = new $class();
			foreach($obj->fields as $field => $type)
			{
				if ($field == $obj->getPrimaryKey()) continue;
	
				$mapping = new DataImportFieldMapping();
				$mapping->class = $class;
				$mapping->client_field = $field;
				$mapping->import_column = "";
				$mapping->import_profile_id = $profile_id;
				
				$mapping->save();
			}
				
			$fieldMappings = Query::create(DataImportFieldMapping, "WHERE class=:cl AND import_profile_id=:pr")
							  ->bind(":cl", $class, ":pr", $profile_id)
							  ->execute();
	
		}
	
		return $fieldMappings;
	}
	
	static function createImportItems($class, $file = "")
	{
		global $user;

		ComponentManager::fireEvent("RegisterSyncHelpers");
		
		if (array_key_exists($class, DataSyncManager::$helpers))
		{
			$helper = DataSyncManager::$helpers[$class];
		}
		
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
	
		foreach($fields as $field)
		{
			$columns[] = $field;
		}
	
		//DataSyncManager::skipToOffset($fp);
		$isChunked = Settings::getValue("data_sync", "import_in_chunks");
		$counter = Settings::getValue("data_sync", "records_per_page");
		$offset = $isChunked ? checkNumeric($_GET["offset"]) : 0;
		
		$lastItem = null;
		
		while($record = fgetcsv($fp))
		{	
			$record = array_combine($columns, $record);
			
			if ($helper)
			{
				$record = $helper->processRecord($record);
			}
			
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
					if ($record[$col])
					{
						$vals[] = $record[$col];
					}
				}
	
				$separator = ($item->fields[$field] == Text) ? "\n" : " ";
	
				$value = trim(implode($separator, $vals));

				if (!$value && $mapping->grouping && $lastItem)
				{
					$value = $lastItem->get($field);
				}
				
				if (!$value && $mapping->required)
				{
					$skip = true;
					break;
				}

				$item->set($field, $value);
			}
			
			if (!$skip) 
			{
				DataSyncManager::$totalRecords++;
				
				if ($isChunked && $offset > 0)
				{
					$offset--;
					continue;
				}
				
				if ($counter <= 0)
				{
					continue;
				}
				
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
				$lastItem = $item;
				$counter--;
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
			if (!is_array($classes))
			{
				$syncTargets[] = $classes;
			}
			
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
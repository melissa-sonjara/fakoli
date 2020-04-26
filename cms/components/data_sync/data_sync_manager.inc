<?php

/**
 * Serialization handler for Link Libraries and Link Records.
 * @author andy
 *
 */
class DataSyncSerializationHandler
{
	function __construct()
	{
	}

	function export()
	{
		$xml  = "\n<DataSyncMap>";
		$xml .= SerializationManager::serialize(DataImportProfile, "ORDER BY import_profile_id");
		$xml .= SerializationManager::serialize(DataImportFieldMapping, "ORDER BY mapping_id");
		$xml .= "</DataSyncMap>";

		return $xml;
	}

	function import($doc, $tx)
	{
		SerializationManager::unserialize(DataImportProfile, $doc, $tx);
		SerializationManager::unserialize(DataImportFieldMapping, $doc, $tx);
	}
}

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

interface IDataSyncAdapter
{
	function uploadDataImportFile($field);
	function countRecords($file);
	function getImportColumns($file);
	function openImportFile($file, &$columns);	
	function skipToOffset($fp, $offset = -1);	
	function getRecord($fp, $columns);
	function closeImportFile($fp);
}

class DataSyncCSVAdapter implements IDataSyncAdapter
{
	
	function uploadDataImportFile($field)
	{
		global $config;
		
		$file = $_FILES[$field]["tmp_name"];
		
		$fullpath = $config["uploadbase"] . DIRECTORY_SEPARATOR . basename($file);
		
		if (file_exists($fullpath))
		{
			unlink($fullpath);
		}
		
		trace("Uploading $file to $fullpath", 3);
		
		move_uploaded_file($file, $fullpath);
		chmod($fullpath, 0755);
		
		return $fullpath;
//		$_SESSION["data_import_file"] = $fullpath;
	}
	
	function countRecords($file)
	{
		//$fp = fopen($file, "r");
		$fp = fopen_utf8($file);
		$counter = -1;
		while(fgetcsv($fp))
		{
			$counter++;
		}
		
		fclose($fp);
		
		return $counter;
	}
	
	function getImportColumns($file)
	{	
		trace("Reading columns for $file", 3);
		//$fp = fopen($file, "r");
		$fp = fopen_utf8($file);
		
		$fields = fgetcsv_noBOM($fp);
		fclose($fp);
		
		$columns = array();
		
		foreach($fields as $field)
		{
			if ($field) $columns[] = $field;
		}
		
		return $columns;
	}
	
	function skipToOffset($fp, $offset = -1)
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
	
	function openImportFile($file, &$columns)
	{
		if (!$file)
		{
			$file = $_SESSION["data_import_file"];
		}
		
		if (!file_exists($file))
		{
			throw new FakoliException("Data Import File does not exist");
		}
		
		trace("Opening import file $file", 3);
		
		//$fp = fopen($file, "r");
		$fp = fopen_utf8($file);
		
		$fields = fgetcsv_noBOM($fp);
		
		foreach($fields as $field)
		{
			$columns[] = $field;
		}
		
		return $fp;
	}
	
	function getRecord($fp, $columns)
	{
		$record = fgetcsv($fp);
		
		if ($record)
		{
			$record = array_combine($columns, $record);
		}
		
		return $record;
	}
	
	function closeImportFile($fp)
	{
		fclose($fp);
	}
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
	static $fieldMappings = array();
	
	static $import_profile_id = 0;
	
	static $adapters = null;
	
	function DataSyncManager()
	{
	}

	static function registerSyncHelper($class, $helper)
	{
		DataSyncManager::$helpers[$class] = $helper;
	}
	
	static function registerDataSyncAdapter($format, $adapter)
	{
		DataSyncManager::$adapters[$format] = $adapter;
	}
	
	static function registerCSVAdapter()
	{
		DataSyncManager::registerDataSyncAdapter("CSV", new DataSyncCSVAdapter());
	}
	
	static function getDataAdapter($format = null)
	{
		if (!$format)
		{
			$format = $_SESSION["data_import_format"];
			if (!$format) $format = "CSV";
		}
		
		if (DataSyncManager::$adapters == null)
		{
			ComponentManager::fireEvent("RegisterDataSyncAdapter");
		}
		
		if (!array_key_exists($format, DataSyncManager::$adapters))
		{
			throw new FakoliException("Unknown Data Format");
		}
		
		return DataSyncManager::$adapters[$format];
	}
	
	static function getAvailableDataAdapters()
	{
		if (DataSyncManager::$adapters == null)
		{
			ComponentManager::fireEvent("RegisterDataSyncAdapter");
		}
		return array_keys(DataSyncManager::$adapters);
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("data_sync", "import_in_chunks", false, Boolean, "Specifies whether to limit the number of records being imported at one time.");
		Settings::setDefaultValue("data_sync", "records_per_page", 500, Number, "The number of records to import at one time when chunked import is enabled.");
		Settings::setDefaultValue("data_sync", "multiple_import_profiles", false, Boolean, "Turn on support for configuring multiple import profile for each sync target.");
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("data_sync", "Data Import Profiles and Field Mappings", new DataSyncSerializationHandler());
		return true;
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
	
	static function countRecords($file = "", $format = null)
	{
		if (!$file)
		{
			$file = $_SESSION["data_import_file"];
		}
		
		$adapter = DataSyncManager::getDataAdapter($format);
		return $adapter->countRecords($file);
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

	static function getHelper($class)
	{
		if (array_key_exists($class, DataSyncManager::$helpers))
		{
			$helper = DataSyncManager::$helpers[$class];
		}
		
		return $helper;
	}
	
	static function getImportColumns($class, $file = "", $format = null)
	{
		ComponentManager::fireEvent("RegisterSyncHelpers");
		$adapter = DataSyncManager::getDataAdapter($format);
		
		if (!$file)
		{
			$file = $_SESSION["data_import_file"];
		}
	
		$columns = $adapter->getImportColumns($file);
	
		$helper = DataSyncManager::getHelper($class);
		if ($helper)
		{
			$columns = $helper->getAdditionalColumns($columns);
		}
		
		return $columns;
	}

	static function extendForm($form)
	{
		$class = get_class($form->data);
		$helper = DataSyncManager::getHelper($class);
		
		if ($helper)
		{
			$helper->extendForm($form);
		}
	}
	
	static function getFieldMappings($class, $profile_id = 0)
	{
		if (!$class) return null;
		
		DataSyncManager::getSyncTargets();
		
		$fieldMappings = IndexedQuery::create(DataImportFieldMapping, "WHERE class=:cl AND import_profile_id=:pr", "client_field")
									 ->bind(":cl", $class, ":pr", $profile_id)
									 ->execute();
	
		// Check for missing fields and create default entries
				
		$obj = new $class();
		foreach($obj->getFields() as $field => $type)
		{
			if ($field == $obj->getPrimaryKey()) continue;
			if (array_key_exists($field, $fieldMappings)) continue;
	
			$mapping = new DataImportFieldMapping();
			$mapping->class = $class;
			$mapping->client_field = $field;
			$mapping->import_column = "";
			$mapping->import_profile_id = $profile_id;
			
			$mapping->save();
		}
				
		$fieldMappings = IndexedQuery::create(DataImportFieldMapping, "WHERE class=:cl AND import_profile_id=:pr", "client_field")
						  ->bind(":cl", $class, ":pr", $profile_id)
						  ->execute();
		
		// Sort field mappings
		$filled = array();
		$empty = array();
		foreach($obj->getFields() as $field => $type)
		{
			if ($field == $obj->getPrimaryKey()) continue;
			if (!array_key_exists($field, $fieldMappings)) continue;
		
			if ($fieldMappings[$field]->import_column)
			{
				$filled[] = $fieldMappings[$field];
			}
			else
			{
				$empty[] = $fieldMappings[$field];
			}
		}
		
		$fieldMappings = array_merge($filled, $empty);
		
		return $fieldMappings;
	}
	

	private static function getMatchingFields($class, $import_profile_id = 0)
	{
		$matchingFields = Query::create(DataImportFieldMapping, "WHERE class=:cl AND import_profile_id=:pr AND matching=1")
		->bind(":cl", $class, ":pr", $import_profile_id)
		->execute();
	
		$mf = array();
	
		foreach($matchingFields as $matchingField)
		{
			$mf[] = $matchingField->client_field;
		}
	
		return $mf;
	}
	
	private static function getMatchingFormat($class, $import_profile_id = 0)
	{
		$mf = DataSyncManager::getMatchingFields($class, $import_profile_id);
		
		$matchingFormat = implode("##", $mf);
		
		return $matchingFormat;
	}
	
	/**
	 * Retrieves an array of the existing items, indexed according to the configured matching fields.
	 * @param string $class the target import class
	 * @return array
	 */
	private static function getExistingItemIDs($class, $import_profile_id = 0)
	{
		$matchingFields = DataSyncManager::getMatchingFields($class, $import_profile_id);
		
		$item = new $class;
		$filter = new InclusionFilter();
		$filter->add($item->getPrimaryKey());
		foreach($matchingFields as $field)
		{
			$filter->add($field);
		}
		
		$existingItems = IteratedQuery::create($class)
									  ->filter($filter)
									  ->execute();
		
		$existingItemIDs = array();
		
		foreach($existingItems as $item)
		{
			$existingItemIDs[$item->format($matchingFormat)] = $item->get($item->getPrimaryKey());
		}
		
		return $existingItemIDs;
	}

	
	static function getFieldMapping($class, $import_profile_id)
	{
		if (array_key_exists($import_profile_id, DataSyncManager::$fieldMappings)) return DataSyncManager::$fieldMappings[$import_profile_id];
		
		trace("Retrieving field mapping for $class profile $import_profile_id", 3);
		
		DataSyncManager::$fieldMappings[$import_profile_id] = 
						IndexedQuery::create(DataImportFieldMapping, "WHERE class=:cl AND import_profile_id=:pr AND import_column != ''", "client_field")
									->bind(":cl", $class, ":pr", $import_profile_id)
									->execute();
		
		return DataSyncManager::$fieldMappings[$import_profile_id];
	}

	static function fieldMappingHasMatchField($fieldMappings)
	{
		foreach($fieldMappings as $fieldMapping)
		{
			if ($fieldMapping->matching) return true;
		}
		
		return false;
	}
	
	static function getActiveFieldMapping($class)
	{
		return DataSyncManager::getFieldMapping($class, DataSyncManager::$import_profile_id);	
	}
	
	static function generateFilter($fieldMapping)
	{
		$filter = new InclusionFilter();
		
		foreach($fieldMapping as $field => $mapping)
		{
			$filter->add($field);
		}
		
		return $filter;
	}
			
	private static function openImportFile($file, &$columns, $format = null)
	{
		$adapter = DataSyncManager::getDataAdapter($format);
		$fp = $adapter->openImportFile($file, $columns);
		
		return $fp;
	}
		
	private static function getRecord($adapter, $fp, $columns, $helper = null, $performLookups = true)
	{
		
		$record = $adapter->getRecord($fp, $columns);

		if ($record)
		{
			if ($helper)
			{
				$record = $helper->processRecord($record, $performLookups);
			}
		}
		
		return $record;
	}
	
	private static function populateItem($item, $record, $fieldMapping, $lastItem = null)
	{
		$skip = false;
		
		//AJG - a bit of a hack to allow SyncHelpers to specify that particular records should be skipped
		if ($record["__skip__"] === true) return true;
		
		foreach($fieldMapping as $field => $mapping)
		{
			if (!$mapping->import_column) continue;
			
			$cols = $mapping->getColumns();

			$vals = array();
		
			foreach($cols as $col)
			{
				if ($record[$col])
				{
					$vals[] = $record[$col];
				}
			}
		
			$separator = ($item->getType($field) == Text) ? "\n" : " ";
		
			$value = trim(implode($separator, $vals));
		
			if (!$value && $mapping->grouped && $lastItem)
			{
				$value = $lastItem->get($field);
			}
		
			if (!$value && $mapping->required)
			{
				trace("Required field $field not set in record. Skipping", 3);
				
				$skip = true;
				break;
			}
		
			$item->set($field, $value);
			//$item->set($field, $item->formatFieldValue($field));
		}
		
		if (!$skip) $item->unpack();
		
		return $skip;
	}
	
	static function createImportItems($class, $import_profile_id = 0, $file = "", $format = null)
	{
		global $user;

		DataSyncManager::$import_profile_id = $import_profile_id;
		
		$item = new $class();
			
		ComponentManager::fireEvent("RegisterSyncHelpers");

		$helper = DataSyncManager::getHelper($class);

		$adapter = DataSyncManager::getDataAdapter($format);
		
		$columns = array();
		
		$fieldMapping = DataSyncManager::getFieldMapping($class, $import_profile_id);
		$filter = DataSyncManager::generateFilter($fieldMapping);
		
		$fp = $adapter->openImportFile($file, $columns);
		
		$items = array();
	
		//DataSyncManager::skipToOffset($fp);
		$isChunked = Settings::getValue("data_sync", "import_in_chunks");
		$counter = Settings::getValue("data_sync", "records_per_page");
		$offset = $isChunked ? checkNumeric($_GET["offset"]) : 0;
		
		$lastItem = null;
		
		$matchingFields = Query::create(DataImportFieldMapping, "WHERE class=:cl AND import_profile_id=:pr AND matching=1")
								->bind(":cl", $class, ":pr", $import_profile_id)
								->execute();
									
		$c = 1;
		$matchingConstraint = "";
			
		foreach($matchingFields as $mf)
		{
			if ($matchingConstraint) $matchingConstraint .= " AND ";
			$matchingConstraint .= "{$mf->client_field}=:{$c}";
			$c++;
		}
		
		$matchingQuery = Query::create($class, "WHERE $matchingConstraint");
		$matchingQuery->filter = new InclusionFilter($item->getPrimaryKey());
			
		while($record = DataSyncManager::getRecord($adapter, $fp, $columns, $helper, ((!$isChunked) || $counter >= 0)))
		{	
			$item = new $class();
			$skip = false;
			
			$item->filter = $filter;

			$skip = DataSyncManager::populateItem($item, $record, $fieldMapping, $lastItem);
			
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
				
				if ($item->hasField("owner_id")) $item->set("owner_id", $user->get($user->getPrimaryKey()));	
				
				$c = 1;
				
				foreach($matchingFields as $mf)
				{
					$matchingQuery->bind(":{$c}", $item->get($mf->client_field));
					$c++;
				}
				
				$match = null;
				try
				{
					$match = $matchingQuery->executeSingle();
					
					$item->set($item->getPrimaryKey(), $match->get($match->getPrimaryKey()));
				}
				catch(DataNotFoundException $e)
				{
				}
				catch(FakoliException $e)
				{
				}
				
				$items[] = $item;
				$lastItem = $item;
				$counter--;
			}
			
		}
	
		fclose($fp);
	
		return $items;
	}
	
	
	static function batchImportItems($class, $import_profile_id, $file, $importMatching, $importNew, $process, $format = null)
	{
		trace("Batching importing $class profile $import_profile_id from $file", 3);
		
		global $user;
		
		DataSyncManager::$import_profile_id = $import_profile_id;
		$adapter = DataSyncManager::getDataAdapter($format);
		
		if (!$process)
		{
			throw new FakoliException("DataSyncManager::batchImportItems cannot be used within a browser request");
		}
		
		try
		{
			ComponentManager::fireEvent("RegisterSyncHelpers");
			
			$helper = DataSyncManager::getHelper($class);
			
			$columns = array();
	
			$process->setProgress("Running", "Scanning file", 0);
			
			$numRecords = DataSyncManager::countRecords($file);
			
			$process->setProgress("Running", pluralize("$numRecords record")." found", 0);
			
			$fieldMapping = DataSyncManager::getFieldMapping($class, $import_profile_id);
			$filter = DataSyncManager::generateFilter($fieldMapping);
	
			$matchingFields = Query::create(DataImportFieldMapping, "WHERE class=:cl AND import_profile_id=:pr AND matching=1")
									->bind(":cl", $class, ":pr", $import_profile_id)
									->execute();
			
			$c = 1;
			$matchingConstraint = "";
			
			foreach($matchingFields as $mf)
			{
				if ($matchingConstraint) $matchingConstraint .= " AND ";
				$matchingConstraint .= "{$mf->client_field}=:{$c}";
				$c++;
			}
	
			trace("Matching Constraint: $matchingConstraint", 3);
			
			$item = new $class();
			
			$fp = $adapter->openImportFile($file, $columns);
			
			$lastItem = null;
			
			$imported = 0;
			$skipped = 0;
			
			$matchingQuery = Query::create($class, "WHERE $matchingConstraint");
			$matchingQuery->filter = new InclusionFilter($item->getPrimaryKey());
			
			while($record = DataSyncManager::getRecord($adapter, $fp, $columns, $helper))
			{
				
				$item = new $class();
				$skip = false;
					
				$item->filter = $filter;
			
				$skip = DataSyncManager::populateItem($item, $record, $fieldMapping, $lastItem);
					
				if (!$skip)
				{
					$c = 1;
						
					foreach($matchingFields as $mf)
					{
						$matchingQuery->bind(":{$c}", $item->get($mf->client_field));
						$c++;
					}
					
					$match = null;
					try
					{
						$match = $matchingQuery->executeSingle();
						
						$item->set($item->getPrimaryKey(), $match->get($match->getPrimaryKey()));
					}
					catch(DataNotFoundException $e)
					{
						// Data not found
					}
					catch(FakoliException $fe)
					{
						// Ambiguous singleton, most likely
					}
					
					if (($match && !$importMatching) || (!$match && !$importNew)) 
					{
						$skipped++;
					}
					else
					{
						$item->save();
						if (method_exists($helper, 'postProcess'))
						{
							$helper->postProcess($item, $record);
						}
						$imported++;
						$lastItem = $item;
					}
				}
				else
				{
					$skipped++;
				}
	
				$counter = $skipped + $imported;
				
				$percentage = intval((100 * $counter) / $numRecords);
				
				$process->setProgress("Running", "Processed record $counter of $numRecords", $percentage);
					
			}
			
			fclose($fp);
	
			$process->setProgress("Completed", "$imported records imported, $skipped records skipped", 100);
			
		}
		catch(Exception $e)
		{
			$process->setProgress("Error", $e->getMessage());
		}
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
		foreach($targets as $index => $classes)
		{
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

class FieldMappingHelper
{
	static function onItemRow($mapping)
	{
		return ($mapping->import_column) ? "mapped" : "";
	}
}

?>
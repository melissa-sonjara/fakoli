<?php
/**\addtogroup component
 * @{
 */


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

/*
 * Parent class for component upgrade management.
 * 

 * 
 * To add self-updating to a component, follow these
 * steps:
 * 
 * 1) create the schema file:
 * 
 *  default sqlFile name is component_name + _schema.sql
 * 
 * e.g. for component "program", the file name would be
 * 	 program_schema.sql

-----------

-- Fakoli MyComponentName Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.0
-- Do not include statement to Drop table if exists.
-- If the table exists, we should not be running this
-- sql.

-- END Version 1.0


-----------

 * 2) In the component's manifest file, subscribe to event
 * "upgradeComponent".

------------
	static function subscribeToEvents()
	{
		return array(
				"upgradeComponent"		=>	array(MyComponentManager, upgradeComponent)
			);
	} 
------------- 
	
 * 3) In your manager class, add the function:
 
-------
 	static function upgradeComponent($version)
	{
		$mgr = new MyComponentUpgradeManager();
		$mgr->upgrade($version);
	}
-------
 * 	4) Create the class MyComponentUpgradeManager:
 * Example: comment component

-------
Fakoli::using("mycomponent", "component");

class MyComponentUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"		=>	"sql:Initial Installation"
	);
	
	function __construct()
	{
		$this->component_name = "mycomponent";

		// sets the sqlFile name according to default
		parent::__construct();		
	}
}
-------

 * 
 * To upgrade the component's data schema or
 * run a data update, child classes enter the 
 * new version in the updateMap as the array key and the
 * update handler as the value. 
 * 
 * Version numbers are the array index in the updateMap - 
 * list version numbers in ascending order.
 * 
 * For the array value, a colon separates the update handler 
 * type from the description (e.g., "sql:my description of my update").
 * 
 * Update Handler Types:
 * 
 * 1) sql - use when only sql statements in the schema file
 * are needed to upgrade the component.
 * 
 * 2) function name (e.g., update_1_1) - use when 
 * The update handler is a custom function which you will
 * write in your component's upgrade manager class.
 * Example:
 * 
-----------------

	function update_1_1($version_number)
	{
		if(!$this->executeVersionSql($version_number))
			return false;
		
		$pages = Query::create(Page, "WHERE published=1")->execute();
		$pageList = formatItems($pages, "'{identifier}'", ",");	
			
		$cPages = Query::create(ComponentPage, "WHERE enabled=1 AND identifier NOT IN ($pageList)")->execute();
			
		$this->addSectionContent($cPages);	
		
		return true;		
	}
---------------------


 * 
 * Update functions must return true if successful so
 * that the update is saved to the log.
 * 
 * 3) done - use this handler type when sql updates are made 
 * locally or have already been made (as in the case of existing 
 * custom web site components, e.g., STEM2Stern programs) you can 
 * use he syntax "done" in place of "sql", then run component scan 
 * to record the update to the ComponentUpdateLog without running 
 * the sql (which has already been run). For local only "done", 
 * remember to change the handler back to "sql" before uploading 
 * the file so that the updates are run on other sites.
 * 
 * Example:
 *  var $updateMap = array(
 *  	"1.0"		=>	"sql:Initial Installation",
 *  	"1.1"		=>	"update_1_1:separate name field into first and last names",
 *  	"1.2"		=>	"sql:add field activity_duration to program table"
 *  );
 *  
 *  
 *  Use of the Log File:
 *  
 *  The $log is used to store details of the upgrade when the
 *  handler is a custom function.
 *  
 *  For example, a function to create missing ledger records might 
 *  write the following details to $this->log:
 *  $this->log .= $xrefPurchase->format("Creating purchase record: registration id: {$xref->conference_participant_xref_id} transaction_id {transaction_id} participant_id {participant_id} conference_id {conference_id} amount {amount} transaction_type {transaction_type} product_code {product_code}\r\n");	
 *  The upgrade manager will create a log file with the contents of $this->log
 *  and store it in the component_update_log record so it can be downloaded
 *  and reviewed, if needed.
*/

abstract class UpgradeManager
{
	var $sqlFile;
	var $xmlDir;
	var $component_name;
	var $log; // output from update functions to store in log file;
		
	/*
	 * Set sqlFile name as component name + "_schema.sql".
	 * First check custom app for match; if not, then check
	 * Fakoli. Custom components take precedence over
	 * Fakoli (e.g., custom user component over Fakoli's user component.
	 */
	function UpgradeManager()
	{
		global $config;
	
		$path = ComponentManager::findComponentPath($this->component_name);		
		$this->sqlFile = $path . DIRECTORY_SEPARATOR . $this->component_name . "_schema.sql";
		$this->xmlDir = $path . DIRECTORY_SEPARATOR . "updates";
	}
	
	
	/*
	* Retrieve the highest component version number listed
 	* for this component in the ComponentUpdateLog.
 	* 
 	* Loop through the updateMap, the history of
 	* updates provided by the component's child class 
 	* update manager, and check the latest version 
 	* against each one. If the latest version is less 
 	* than the update, then execute the update and save 
 	* the record of this update in the ComponentUpdateLog 
 	* table.
	 */
	function upgrade($upgrade_to = "")
	{					
		if ($upgrade_to)
		{
			ComponentManager::scanTrace("Upgrading {$this->component_name} to version $upgrade_to", 2);
		}
		
		$latest_version = ComponentUpdateLog::getLatestVersion($this->component_name);
		ComponentManager::scanTrace("UpgradeManager:: upgrade: latest version in db for component {$this->component_name} is $latest_version", 3);
					
		if(count($this->updateMap) > 0)
		{
			foreach($this->updateMap as $version_number => $handler)
			{
				// remove any contents from a previous upgraded version
				$this->log = "";
				if($latest_version < $version_number)
				{
					list($function, $description) = explode(":", $handler);
					if($this->upgradeOneVersion($version_number, $function))
						$this->recordUpdate($version_number, $description);
					else
						break;
				}
				
				if ($upgrade_to != "" && $upgrade_to == $version_number) break;
			}
		}
				
		// Save the latest version number to the version
		// field of the component record
		ComponentManager::setComponentVersion($this->component_name);
	}
	
	/*
	 * If the update is just sql, then parse the
	 * sql file for the updates in the version number's 
	 * section. 
	 * 
	 * If the update handler is "done", then just return
	 * true.
	 * 
	 * If the handler is a custom functionin the child
	 * class, then execute that function.
	 */	
	function upgradeOneVersion($version_number, $function)
	{			
		if($function == "sql")
			return $this->executeVersionSql($version_number);
		else if ($function == "xml")
			return $this->executeVersionXML($version_number);
		elseif($function != "done")
		{		
			$updater = array($this, $function);
			return call_user_func($updater, $version_number);
		}
		else // sql update done in local copy
			return true;
	}
		
	/*
	 * Execute the section of the component's sql update file
	 * that relations to the specified version number.
	 * 
	 * $version_number - the version number that identifies
	 * the block of sql in the file to be executed.
	 * e.g., "1.1"
	 * 
	 * The sql file should have the commented markers:
	 * 
	 * -- START version 1.1
	 * ... sql statements
	 * -- END version 1.1
	 */	
	function executeVersionSql($version_number)
	{
		ComponentManager::scanTrace("** Upgrade {$this->component_name} to Version $version_number", 2);
		
		$lines = array();
		if(!file_exists($this->sqlFile))
		{
			ComponentManager::scanTrace("UpgradeManager:: sql file does not exists {$this->sqlFile}", 2);
			return;
		}
		
		$fp = fopen($this->sqlFile, 'r');
		$version_id = str_ireplace(".", "\\.", "version " . $version_number); // case insensitive
		trace("UpgradeManager::version id: $version_id file $this->sqlFile", 3);
		
		if(!$fp)
		{
			ComponentManager::scanTrace("Error: executeVersionSql failed to open file", 2);
			return;
		}
		
		while (($buffer = fgets($fp)) !== false && !preg_match("/^--.*?\s+$version_id/i", $buffer)) 
			continue;
				
		// Found start of this verson's update section in the sql file
		if(preg_match("/^--\s*START\s*$version_id/i", $buffer))
		{
			while(($buffer = fgets($fp)) !== false && !preg_match("/^--\s*END\s*$version_id/i", $buffer))
			{
				$depends = array();
				if (preg_match('/^--\s*Depends\s+On\s+([\w_]+)\s+(.*)/i', $buffer, $depends))
				{
					ComponentManager::scanTrace("{$this->component_name} $version_id is dependent on {$depends[1]} {$depends[2]}", 2);
					
					UpgradeManager::upgradeComponentToVersion($depends[1], trim($depends[2]));
				}
				
				// Omit blank lines and comments
				if(trim($buffer) == "" || preg_match("/^--(.*?)\s/", $buffer))
					continue;
				$lines[] = $buffer;
			}
			
			trace("** Upgrade has ".count($lines)." lines", 3);
		}
		
		$sqlStatements = $this->parseSQLStatements($lines);

		if(count($sqlStatements) == 0)
		{
			ComponentManager::scanTrace("UpgradeManager:: no query statements found for version $version_number", 3);
			return true;
		}

		try
		{
			$rtn = $this->executeSQLStatements($sqlStatements);
			return rtn;
		}
		catch(Exception $e)
		{
			ComponentManager::scanTrace("Failed to upgrade {$this->component_name} to version $version_number - ". $e->getMessage(), 2);
			throw new FakoliException("Failed to upgrade {$this->component_name} to version $version_number - ". $e->getMessage());
		}
	}
	
	/*
	 * Create an array of valid sql statements
	 * by scanning for ";"	
	 */
	function parseSQLStatements($lines)
	{
		if(count($lines) == 0)
			return;
			
		$delimiter = ";";
		foreach($lines as $line)
		{
			$d = array();

			ComponentManager::scanTrace("UpgradeManager:: processing update schema line $line", 4);
			if (preg_match("/^\s*DELIMITER\s+(.*?)$/i", $line, $d))
			{
				$delimiter = preg_quote($d[1]);
				//$sqlStatement .= $line."\n";
			}
			else
			{
				if(preg_match("/{$delimiter}\s*$/", $line))
				{
					$sqlStatement .= preg_replace("/{$delimiter}\s*$/", "", $line);
					$sqlStatements[] = $sqlStatement;
					$sqlStatement = "";
				}
				else
				{
					$sqlStatement .= $line;				
				}
			}
		}
		trace(print_r($sqlStatements, true), 3);
		return $sqlStatements;		
	}
	
	function executeSQLStatements($sqlStatements)
	{
		try
		{
			trace("Executing SQL Statements", 2);
			
			$db = ConnectionManager::getConnection();
			$db->beginTransaction();
	
			foreach($sqlStatements as $sqlStatement)
			{
				trace($sqlStatement, 2);
				$db->exec($sqlStatement);	
			}

			$db->commit();
			fclose($fp);	
		}
		catch(PDOException $e)
		{
			$db->rollBack();
			fclose($fp);
			throw $e;	
		}

		return true;
	}
	
	function log($text)
	{
		if(preg_match("/(<br>|<br\/>)$/i", $text))
			$this->log .= preg_replace(array("/<br>$/i", "/<br\/>$/i"), "\r\n", $text);
		else
			$this->log .= $text . "\r\n";
	}
	
	function executeVersionXML($version)
	{
		global $config;
		
		ComponentManager::scanTrace("** Upgrade {$this->component_name} to Version $version_number", 3);
		
		if (preg_match("/\\b{$this->component_name}\\b/", $config["ignore_xml_updates"]))
		{
			ComponentManager::scanTrace("** Ignoring this upgrade as specified in config", 3);
			return true;
		}
		
		if (!is_dir($this->xmlDir))
		{
			throw new FakoliException("XML update directory does not exists for {$this->component_name}");
		}
		
		$file = $this->xmlDir . DIRECTORY_SEPARATOR . $this->component_name ."_" . $version . ".xml";
		
		if (!file_exists($file))
		{
			throw new FakoliException("No XML update for version $version of $this->component_name");
		}
		
		$xml = file_get_contents($file);
		$mgr = new SerializationManager();
		$mgr->importAll($xml);
		
		return true;
	}
	
	function recordUpdate($version, $description)
	{
		if($this->log)
			$fileName = $this->saveLogFile($version);
		ComponentUpdateLog::recordUpdate($this->component_name, $version, $description, $fileName);
	}	
	
	function saveLogFile($version)
	{
		$upgradePath = Settings::getValue("component", "upgrade_output_path");
	
		$fileName = $this->component_name . "_" . $version . ".txt";
		$fp = fopen($upgradePath . DIRECTORY_SEPARATOR . $fileName, 'w');
		fwrite($fp, $this->log);
	    fclose($fp);
	    return $fileName;
	}
	
		
	/**
	 * Utility function for use by calling classes to
	 * add an array of items of class Page, ComponentPage,
	 * Blog, ImageGallery, DocumentLibrary, Calendar 
	 * to a section. 
	 * 
	 * @items can be any of the following:
	 * 1) array of CMS Page, Component Page, Blog, ImageGallery,
	 * DocumentLibrary, or Calendar DataItem objects
	 * 2) a single DataItem obj of any of the above types
	 * 3) array of CMS Page or Component page identifiers
	 * 4) a single CMS Page or Component Page identifier

	 * Alternatively, $items can be an array of
	 * identifiers or a single identifier as long as 
	 * the identifiers are a CMS Page or Component Page.
	 * 
	 * We find the section based on the section name,
	 * and query the existing content to avoid adding
	 * the identifier twice.
	 * 
	 * If the page is found, its role and template,
	 * are updated, if an update is provided.
	 */
	function addSectionContent($items, $section_name = "/", $role = "", $template = "", $permissions = "")
	{
		if($items && !is_array($items))
			$items = array($items);
			
		if(count($items) == 0)
		{
			trace("No items found to add to section", 2);
			return true;
		}
		
		$sections = Query::create(Section, "WHERE section=:section")
		->bind(":section", $section_name)
		->execute();
			
		if(count($sections) == 0)
		{
			trace("UpgradeManager::addSectionContent - section $section_name not found", 3);
			return false;
		}
		else
			$section = $sections[0];
			
		foreach($items as $item)
		{
			if (is_object($item))
			{
				$ident = $item->identifier;
			}
			else
			{
				$ident = $item;
			}
			
			// should be only one or none
			$contents = Query::create(SectionContent, "WHERE identifier=:identifier AND section_id=:section_id")
				->bind(":identifier", $ident, ":section_id", $section->section_id)
				->execute();
				
			$content = (count($contents) > 0) ? $contents[0] : null;

			if(!$content)
			{
				$content = new SectionContent();
				$content->section_id = $section->section_id;
				$content->identifier = $ident;
			}
			else
			{
				$content->filter = new InclusionFilter("template", "role");
			}

			if($section->default_template != $template)
				$content->template = $template;
			if($section->default_role != $role)
				$content->role = $role;
			if ($section->default_permissions != $permissions)
				$content->permissions = $permissions;
							
			$content->save();
			
			trace("Added $item to $section_name", 3);
		}
		
		return true;
	}
	
	
	/**
	 * Given an array of either identifiers that must be
	 * either CMS Page or ComponentPage identifiers or items 
	 * that are of obj CMSPage, Blog, Calendar, etc., link each
	 * to the specified module.
	 */
	function addModuleToPages($items, $module_name, $position = 'right', $sort_order = 1)
	{
		if($items && !is_array($items))
			$items = array($items);
			
		if(count($items) == 0)
			return true;
			
		$modules = Query::create(Module, "WHERE title=:title")
		->bind(":title", $module_name)
		->execute();
			
		if(count($modules) == 0)
			return false;
		else
			$module = $modules[0];

		foreach($items as $item)
		{
			if(!is_object($item))
			{
				$page = $this->searchByIdentifier($item);
				if(!$page) return false;
			}
			else
				$page = $item;

			$xref_class = (preg_match("/component/i", (get_class($page)))) ? ComponentPageModuleXref : PageModuleXref; 
			$xref = new $xref_class();
			$pk = $page->getPrimaryKey();

			$found = Query::create($xref_class, "WHERE $pk=:$pk AND module_id=:module_id")
				->bind(":$pk", $page->$pk, ":module_id", $module->module_id)
				->executeValue("COUNT(1)");
		
			if(!$found)
			{
				$xref->$pk = $page->$pk;
				$xref->module_id = $module->module_id;
				$xref->position = $position;
				$xref->sort_order = $sort_order;
				$xref->save();
				$this->log .= $page->format("Adding $xref_class record for page {identifier} to module {$module->title}\r\n");	
			}
			else
			{
				$this->log .= "Page identifier $identifier already linked to module {$module->title}\r\n";
			}	
		}
			
		return true;
	}
	
	/*
	 * We don't use "findByIdentifier" because would
	 * crash if not found.
	 */
	function searchByIdentifier($identifier)
	{
		$pages = Query::create(Page, "WHERE identifier=:identifier")
				->bind(":identifier", $identifier)
				->execute();
				
		if(count($pages) == 0)
		{
			$pages = Query::create(ComponentPage, "WHERE identifier=:identifier")
			->bind(":identifier", $identifier)
			->execute();
		}
			
		if(count($pages) == 0)
		{
			$this->log .= "Page identifier $identifier not found in Page or ComponentPage tables\r\n";
		}	
			
		return (count($pages) > 0) ? $pages[0] : null;			
	}
	
	/*
	 * @item - page of class CMS Page, Component Page, 
	 * Calendar, etc. If an identifier is sent, it must
	 * be of class CMS Page or Component Page. 
	 * We need the full object b/c we need to set the menu 
	 * item's role using the object's roles
	 * 
	 * @menu_identifier - identifier of menu in table "menu" to which the item should be
	 * added (e.g., "personnel_subnav", "global")
	 * 
	 * @parent_identifier - the identifier of the menu_item in menu_item table
	 * under which this menu item should be added - empty if top level
	 * 
	 * @title - the title to give to the menu item, if not supplied
	 * then prettify the identifier
	 * 
	 * @section_name - the identifier of the section that belongs in 
	 * the url path to the page from this menu item. If not supplied, 
	 * the default is the the identifier of the first section the page 
	 * is linked to in section_content. This function uses the section 
	 * to build the url field stored with the menu item in the format: 
	 * section / page identifier (e.g., /global/programs).
	 * 
	 * @sort_order - the desired sort order for this item; if empty
	 * then set to bottom.
	 * 
	 */
	function addMenuItem($item, $menu_identifier, $parent_identifier = "", $title = "", $section_name = "", $sort_order = 0)
	{				
		// Find the menu
		$menus = Query::create(Menu, "WHERE identifier=:identifier")
			->bind(":identifier", $menu_identifier)
			->execute();
			
		if(count($menus) == 0)
			return false;
		else
			$menu = $menus[0];
			
		if(!is_object($item))
		{
			$page = $this->searchByIdentifier($item);
			if(!$page) return false;
		}
		else
			$page = $item;
						
		$found = Query::create(MenuItem, "WHERE (identifier=:identifier OR url like '%{$page->identifier}') AND menu_id=:menu_id")
			->bind(":identifier", $page->identifier, ":menu_id", $menu->menu_id)
			->executeValue("COUNT(1)");

		if($found)
		{
			$this->log .= "Page identifier {$page->identifier} already linked to menu {$menu_identifier}\r\n";
			return true;
		}
		
		$parent_id = 0;
		
		if($parent_identifier)
		{
			$parents = Query::create(MenuItem, "WHERE identifier=:identifier")
				->bind(":identifier", $parent_identifier)
				->execute();

			if(count($parents) > 0)
				$parent_id = $parents[0]->menu_item_id;
		}
		
		if(!$sort_order)
		{
			$sort_order = (Query::create(MenuItem, "WHERE menu_id=:menu_id AND parent_id=:parent_id")
				->bind(":menu_id", $menu->menu_id, ":parent_id", $parent_id)
				->executeValue("MAX(sort_order)") + 1);
		}
		
		if(!$section_name)
		{
			$sections = Query::create(SectionContent, "WHERE identifier=:identifier")
				->bind(":identifier", $page->identifier)
				->execute();

			if(count($sections) == 0)
			{
				trace("UpgradeManager error: page {$page->identifier} is not in section content", 3);
				return false;	
			}

			$section_name = $sections[0]->Section()->section;
		}
		
		$menuItem = new MenuItem();
		$menuItem->title = ($title) ? $title : prettify($page->identifier);
		$menuItem->menu_id = $menu->menu_id;
		$menuItem->parent_id = $parent_id;
		$menuItem->published = 1;
		$menuItem->url =  "/" . $section_name . "/" . $page->identifier;
		$menuItem->role = $page->role;
		$menuItem->identifier = $page->identifier;
		$menuItem->sort_order = $sort_order;
		$menuItem->page_id = 0;
		$menuItem->save();
		
		$this->log .= $page->format("Adding menu item {identifier} to menu {$menu->name}\r\n");	
	
		return true;
	}
	
	/*
	 * reformat a string phone field to remove
	 * nondigits so that it can use the PhoneFieldRenderer
	 * 
	 * @param item - obj of DataItem class
	 * 
	 * @param field - field name to reformat
	 */
	function reformatPhone(&$item, $field)
	{
		$oldPhone = $item->$field;
		if(!$oldPhone)
			return;

		// remove the 1 before 800 or 888
		$phone = preg_replace("/^1-/","", $oldPhone);
		// remove all nondigits
		$phone = preg_replace("/\D/","", $phone);

		if($phone != $oldPhone)
		{
			$item->$field = $phone;
			$table = $item->table;
			$pk = $item->getPrimaryKey();
			$this->log($item->format("Setting {$item->table} {$pk} $field from {$oldPhone} to {$phone}"));
			$item->filter = new InclusionFilter($field);
			$item->save();
		}	
	}	

	/*
	 * Use this function to execute sql in a file
	 * that is not the main compoennt's sql schema file.
	 * Needed when the data is too large to paste into the
	 * schema file.
	 * 
	 * @param sqlFile - full path to sqlFile. File can only contain 
	 * "-- " style for comments, not "/*"
	 * 
	 * @param - version_number - the version number of the calling
	 * update function
	 */
	function executeSQLFile($sqlFile, $version_number)
	{
		trace("** Upgrade {$this->component_name} to version $version_number by executing file {$sqlFile}", 3);
		
		if(!file_exists($sqlFile))
		{
			trace("UpgradeManager:: sql file does not exists {$sqlFile}", 2);
			return;
		}
		
		$fp = fopen($sqlFile, 'r');
		
		if(!$fp)
		{
			trace("Error: executeVersionSql failed to open file", 3);
			return;
		}
						
		$lines = array();
		while(($buffer = fgets($fp)) !== false)
		{
			// Omit blank lines and comments
			if(trim($buffer) == "" || preg_match("/^--(.*?)\s/", $buffer))
				continue;
			$lines[] = $buffer;
		}
		
		if(count($lines) == 0)
		{
			trace("UpgradeManager:: no lines parsed in file {$sqlFile}", 3);
			return true;		
		}
			
		$sqlStatements = $this->parseSQLStatements($lines);

		if(count($sqlStatements) == 0)
		{
			trace("UpgradeManager:: no query statements found in sql file {$sqlFile} for version $version_number", 3);
			return true;
		}

		return $this->executeSQLStatements($sqlStatements);
	}
	
	function dependsOn($component, $version)
	{
		UpgradeManager::upgradeComponentToVersion($component, $version);	
	}
	
	static function upgradeComponentToVersion($component, $version)
	{
		trace("Firing upgrade event to $component, target version $version", 3);
		ComponentManager::fireEventTo("upgradeComponent", $component, $version);
	}
}
/**@}*/?>

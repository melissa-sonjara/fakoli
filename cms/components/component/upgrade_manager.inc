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
 	static function upgradeComponent()
	{
		$mgr = new MyComponentUpgradeManager();
		$mgr->upgrade();
	}
-------
 * 	4) Create the class MyComponentUpgradeManager:
 * Example: comment component

-------
Fakoli::using("comment", "component");

class CommentUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"		=>	"sql:Initial Installation"
	);
	
	function CommentUpgradeManager()
	{
		$this->component_name = "comment";

		// sets the sqlFile name according to default
		parent::UpgradeManager();		
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
	
		$component = Component::getComponent($this->component_name);		
		$this->sqlFile = $component->component_path . DIRECTORY_SEPARATOR . $this->component_name . "_schema.sql";
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
	function upgrade()
	{					
		$latest_version = ComponentUpdateLog::getLatestVersion($this->component_name);
		trace("UpgradeManager:: upgrade: latest version in db for component {$this->component_name} is $latest_version", 3);
					
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
		$lines = array();
		if(!file_exists($this->sqlFile))
		{
			trace("UpgradeManager:: sql file does not exists {$this->sqlFile}", 3);
			return;
		}
		
		$fp = fopen($this->sqlFile, 'r');
		$version_id = "version " . $version_number; // case insensitive
		trace("UpgradeManager::version id: $version_id file $this->sqlFile", 3);
		
		if(!$fp)
		{
			trace("Error: executeVersionSql failed to open file", 3);
			return;
		}
		
		while (($buffer = fgets($fp)) !== false && !preg_match("/^--\s.*?$version_id/i", $buffer)) 
			continue;
				
		// Found start of this verson's update section in the sql file
		if(preg_match("/^--\s.*?$version_id/i", $buffer))
		{
			while(($buffer = fgets($fp)) !== false && !preg_match("/^--\s.*?$version_id/i", $buffer))
			{
				// Omit blank lines and comments
				if(trim($buffer) == "" || preg_match("/^--(.*?)\s/", $buffer))
					continue;
				$lines[] = $buffer;
			}
		}
		
		// Create an array of valid sql statements
		// by scanning for ";"	
		if(count($lines) > 0)
		{
			foreach($lines as $line)
			{
				trace("UpgradeManager:: processing update schema line $line", 3);
				$sqlStatement .= $line;
				if(preg_match("/;\s$/", $line))
				{
					$sqlStatements[] = $sqlStatement;
					$sqlStatement = "";
				}
			}
		}

		if(count($sqlStatements) == 0)
		{
			trace("UpgradeManager:: no query statements found for version $version_number", 3);
			return true;
		}

		try
		{
			$db = ConnectionManager::getConnection();
			$db->beginTransaction();
	
			foreach($sqlStatements as $sqlStatement)
			{
				trace("UpgradeManager:: executing statement: $sqlStatement", 3);
				$db->exec($sqlStatement);	
			}

			$db->commit();
			fclose($fp);	
		}
		catch(PDOException $e)
		{
			$db->rollBack();
			fclose($fp);
			ajaxReturn("SQL execute version update failed - ".$e->getMessage());	
		}
	
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
	
	/*
	 * Utility function for use by calling classes to
	 * add an array of items of class Page, ComponentPage,
	 * Blog, ImageGallery, DocumentLibrary, Calendar 
	 * to a section.
	 * 
	 * Alternatively, $items can be an array of
	 * identifiers as long as the identifiers are
	 * a CMS Page or Component Page.
	 * 
	 * We find the section based on the section name,
	 * and query the existing content to avoid adding
	 * the identifier twice.
	 */
	function addSectionContent($items, $section_name = "/")
	{
		if($items && !is_array($items))
			$items = array($items);
			
		if(count($items) == 0)
			return true;
					
		$sections = Query::create(Section, "WHERE section=:section")
		->bind(":section", $section_name)
		->execute();
			
		if(count($sections) == 0)
			return false;
		else
			$section = $sections[0];
			
		foreach($items as $item)
		{
			if(!is_object($item))
			{
				$page = $this->searchByIdentifier($item);
				if(!$page) return false;
			}
			else
				$page = $item;
			
			$found = Query::create(SectionContent, "WHERE identifier=:identifier AND section_id=:section_id")
			->bind(":identifier", $page->identifier, ":section_id", $section->section_id)
			->executeValue("COUNT(1)");

			if(!$found)
			{
				$content = new SectionContent();
				$content->section_id = $section->section_id;
				$content->identifier = $page->identifier;
				$content->template = $page->template;
				$content->role = $page->role;
				$content->save();
				$this->log .= $page->format("Adding page {identifier} to section {$section->section_title}\r\n");
			}
		}

		return true;
	}
	
	/*
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
	 * $item - page of class CMS Page, Component Page, 
	 * Calendar, etc. If an identifier is sent, it must
	 * be of class CMS Page or Component Page. 
	 * We need the full object b/c we need to set the menu 
	 * item's role using the object's roles
	 * 
	 * $menu_name - name of menu to which the item should be
	 * added
	 *
	 * $title - the title to give to the menu item, if not supplied
	 * then prettify the identifier
	 * 
	 * $parent_menu - the title of the menu_item under which this menu item 
	 * should be added - empty if top level
	 * 
	 * $sort_order - the desired sort order for this item; if empty
	 * then set to bottom.
	 * 
	 */
	function addMenuItem($item, $menu_name, $title = "", $parent_menu = "", $sort_order = 0)
	{				
		// Find the menu
		$menus = Query::create(Menu, "WHERE name=:name")
			->bind(":name", $menu_name)
			->execute();
			
		if(count($menus) == 0)
		{
			return false;
		}
		else
			$menu = $menus[0];
			
		if(!is_object($item))
		{
			$page = $this->searchByIdentifier($item);
			if(!$page) return false;
		}
		else
			$page = $item;
			
			
		$found = Query::create(MenuItem, "WHERE identifier=:identifier AND menu_id=:menu_id")
			->bind(":identifier", $page->identifier, ":menu_id", $menu->menu_id)
			->executeValue("COUNT(1)");

		if($found)
		{
			$this->log .= "Page identifier {$page->identifier} already linked to menu {$menu_name}\r\n";
			return true;
		}
		
		$parent_id = 0;
		
		if($parent_menu)
		{
			$parents = Query::create(MenuItem, "WHERE title=:title")
				->bind(":title", $parent_menu)
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
		
		$menuItem = new MenuItem();
		$menuItem->title = ($title) ? $title : prettify($page->identifier);
		$menuItem->menu_id = $menu->menu_id;
		$menuItem->parent_id = ($parent) ? $parent->menu_item_id : 0;
		$menuItem->published = 1;
		$menuItem->role = $page->role;
		$menuItem->identifier = $page->identifier;
		$menuItem->sort_order = $sort_order;
		$menuItem->page_id = 0;
		$menuItem->save();
		
		$this->log .= $page->format("Adding menu item {identifier} to menu {$menu->name}\r\n");	
	
		return true;
	}
}
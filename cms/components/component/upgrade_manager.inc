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
 * default sqlFile name is component_name + _schema.sql
 * 
 * e.g., program_schema.sql
 * 
 * To add self-updating to a component, follow these
 * steps:
 * 
 * 1) In the component's manifest file, subscribe to event
 * "upgradeComponent".
 * 
	static function subscribeToEvents()
	{
		return array(
				"upgradeComponent"		=>	array(MyComponentManager, upgradeComponent)
			);
	}  
	
 * 2) In your manager class, add the function:
 * 
 	static function upgradeComponent()
	{
		$mgr = new MyComponentUpgradeManager();
		$mgr->upgrade();
	}
	
 * 	3) Create the class MyComponentUpgradeManager
 * 	using an existing class as a template.
 * 
 * To upgrade the component's data schema or
 * run a data update, child classes enter the 
 * new version in the updateMap as the array key and the
 * update handler as the value. 
 * 
 * List version numbers in ascending order.
 * For the value, a colon separates the update handler 
 * from the description.
 * 
 * The update handler is either the function name in this
 * class that handles the update or, when only
 * sql needs to be executed (no code) then the handler is:
 * 	"sql:description". You must save the sql update 
 * statements in the {component_name} + _schema.sql file.
 * 
 * Update functions must return true if successful so
 * that the update is saved to the log.
 * 
 * When sql updates are made locally or have already
 * been made (as in the case of existing custom web site 
 * components, e.g., STEM2Stern programs) you can use
 * the syntax "done" in place of "sql", then run
 * component scan to record the update to the 
 * ComponentUpdateLog without running the sql (which has
 * already been run). For local only "done", remember to 
 * change the handler back to "sql" before uploading 
 * the file so that the updates are run on other sites.
 *
 * Example:
 *  var $updateMap = array(
 *  	"1.0"		=>	"sql:Initial Installation",
 *  	"1.1"		=>	"update_1_1:separate name field into first and last names",
 *  	"1.2"		=>	"sql:add field activity_duration to program table"
 *  );
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

Fakoli::using("component", "section");

require_once realpath(dirname(__FILE__)."/../section/datamodel/section.inc");

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
	
		$fakoliFile = realpath(dirname(__FILE__)."/../" . $this->component_name ."/". $this->component_name . "_schema.sql");
		$siteFile = $config['homedir'] . "/" . "components" ."/".  $this->component_name ."/". $this->component_name . "_schema.sql";
		$this->sqlFile = (file_exists($siteFile)) ? $siteFile : $fakoliFile;
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
	 * We find the section based on the section name,
	 * and query the existing content to avoid adding
	 * the identifier twice.
	 */
	function addSectionContent($items, $section_name = "/")
	{
		if(count($items) == 0)
			return true;
			
		$section = Section::findSection($section_name);
		if(!$section)
			return false;
			
		foreach($items as $item)
		{
			$found = Query::create(SectionContent, "WHERE identifier=:identifier AND section_id=:section_id")
				->bind(":identifier", $item->identifier, ":section_id", $section->section_id)
				->execute();
				
			if(count($found) == 0)
			{
				$content = new SectionContent();
				$content->section_id = $section->section_id;
				$content->identifier = $item->identifier;
				$content->template = $item->template;
				$content->role = $item->role;
				$content->save();
			}
		}
		
		return true;
	}	
}
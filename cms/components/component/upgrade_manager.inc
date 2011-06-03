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


abstract class UpgradeManager
{
	var $sqlFile;
	var $component_name;
	/*
	 * Child classes create updateMap to map version number 
	 * to updater function.
	 * List version numbers in ascending order
	 * 
	 *  var $updateMap = array(
	 *  	"1.1"		=>	"update_1_1"
	 *  );
	 *  	
	*/
	
	function UpgradeManager()
	{
		// child classes set:
		// $this->sqlFile = xx;
		// $this->component_name = xxx;
	}
	
	function upgrade()
	{					
		$latest_version = ComponentUpdateLog::getLatestVersion($this->component_name);
		trace("UpgradeManager:: upgrade: latest version in db for component {$this->component_name} is $latest_version", 3);
					
		if(count($this->updateMap) > 0)
		{
			foreach($this->updateMap as $update => $function)
			{
				if($latest_version < $update)
				{
					$updater = array($this, $function);
					$rtn = call_user_func($updater, $update);
					if(!$rtn)
						break;
				}
			}
		}
				
		// Save the latest version number to the version
		// field of the component record
		ComponentManager::setComponentVersion($this->component_name);
	}
		
	/*
	 * Execute the section of the component's sql update file
	 * that relations to the specified version number.
	 * 
	 * $fp - file pointer opened in the calling class
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
		$fp = fopen($this->sqlFile, 'r');
		
		if(!$fp)
		{
			trace("Error: executeVersionSql failed to open file", 3);
			return;
		}
		
		while (($buffer = fgets($fp)) !== false && !preg_match("/version $version_number/i", $buffer)) 
			continue;

		if(preg_match("/version $version_number/i", $buffer))
		{
			while(($buffer = fgets($fp)) !== false && !preg_match("/end version $version_number/i", $buffer))
			{
				// Omit blank lines and comments
				if(!$buffer || preg_match("/^--(.*?)\s/", $buffer)) continue;
				$lines[] = $buffer;
			}
		}
		
		// Create an array of valid sql statements
		// by scanning for ";"	
		if(count($lines) > 0)
		{
			foreach($lines as $line)
			{
				$sqlStatement .= $line;
				if(preg_match("/^(.*?)\s;\s$/", $line))
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
		ComponentUpdateLog::recordUpdate($this->component_name, $version, $description);
	}	
	
	/*
	 * Temporary function to set the log to reflect
	 * that this component has already been rolled out
	 * for this environment. Only needed until all
	 * workspaces have been initialized to version 1.0,
	 * inittial roll-out.
	 * 
	 * We need this to avoid dropping and recreating
	 * tables that already exist and have data.
	 */
	function initializeLog()
	{
		$latest_version = ComponentUpdateLog::getLatestVersion($this->component_name);

		if(!$latest_version)
			$this->recordUpdate("1.0", "Initial Installation");
	}
	
}
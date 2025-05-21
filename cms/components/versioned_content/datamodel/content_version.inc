<?php
/**\addtogroup versioned_content
 * @{
 */



class ContentVersion extends DataItem
{
	var $table = "content_version";
	var $primary_key = "content_version_id";
	
	var $fields = array("content_version_id"	=>	'Number',
						"content_class"			=>	'String',
						"content_id"			=>	'Number',
						"version_number"		=>	'Number',
						"content"				=>	'Text',
						"last_modified"			=>	'DateTime',
						"last_modified_by"		=>	'Number',
						"approved"				=>	'Boolean',
						"approved_date"			=>	'DateTime',
						"approved_by"			=>	'Number');

	/**
	 * Saves a draft version of the target object to the content version table
	 * @param DataItem $target the target to save
	 */
	static function saveDraft($target)
	{
		global $user;
		
		$tx = new DataTransaction();
		try
		{
			try 
			{
				$contentVersion = Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i AND approved=0")
									   ->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue())
									   ->executeSingle();
			} 
			catch (DataNotFoundException $e) 
			{
				$contentVersion = new ContentVersion();
				$contentVersion->content_class = get_class($target);
				$contentVersion->content_id = $target->getPrimaryKeyValue();
				$contentVersion->version_number = ContentVersion::nextVersion($target, $tx);
				$contentVersion->approved = false;
				$contentVersion->approved_by = 0;
			}
			
			$contentVersion->last_modified_by = $user->getPrimaryKeyValue();
			$contentVersion->last_modified = now();
			
			$contentVersion->populateFrom($target);
			
			$contentVersion->joinTransaction($tx);
			$contentVersion->save();
			
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
		}
	}
	
	/**
	 * Update the versioned fields on target object to the current draft version.
	 * @param DataItem $target the target content object
	 */
	static function loadDraft($target)
	{
		try
		{
			trace(print_r($target, true), 3);
			
			$contentVersion = Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i AND approved=0")
								   ->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue())
								   ->executeSingle();
			
			$contentVersion->populateTo($target);
		}
		catch(DataNotFoundException $e)
		{
			trace("Draft not found", 3);
			// Nothing to do
		}
		
	}
	
	/**
	 * Determine whether the specified target content item has a draft version outstanding
	 * @param DataItem $target the target content item
	 * @return true if a draft version has been created, false otherwise
	 */
	static function hasDraft($target)
	{
		return Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i AND approved=0")
					->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue())
					->exists();
	}
	
	/**
	 * Delete the current draft changes for the specified target (if they exist)
	 * @param DataItem $target
	 */
	static function deleteDraft($target)
	{
		try
		{
			$doomed = Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i AND approved=0")
							->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue())
							->executeSingle();
			$doomed->delete();
		}
		catch(DataNotFoundException $e)
		{
			trace("Draft not found", 3);
			// Nothing to do
		}
		
	}
	
	/**
	 * Returns an array of previously approved versions of this content item
	 * @param DataItem $target the target content item
 	 * @param boolean $includeContent true to return the content from previous versions, false to exclude content
	 */
	static function approvedVersions($target, $includeContent = false)
	{
		$query = Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i AND approved=1 ORDER BY version_number DESC")
					  ->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue());
		
		if (!$includeContent)
		{
			$query->filter(new ExclusionFilter("content"));
		}
		
		return $query->execute();
	}

	static function getVersion($target, $version)
	{
		if ($version == "draft")
		{
			// Get current draft
			try
			{
				$contentVersion = Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i AND approved=0")
									   ->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue())
									   ->executeSingle();
			}
			catch(DataNotFoundException $e)
			{
				$contentVersion = null;
			}
		}
		else if ($version == "")
		{
			try
			{
				$contentVersion = Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i AND approved=1 ORDER BY version_number DESC LIMIT 1")
									   ->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue())
									   ->executeSingle();
			}
			catch(DataNotFoundException $e)
			{
				$contentVersion = null;
			}
		}
		else
		{
			try
			{
				$contentVersion = Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i AND version_number=:v AND approved=1")
										->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue(), ":v", $version)
										->executeSingle();
			}
			catch(DataNotFoundException $e)
			{
				$contentVersion = null;
			}
		}
		
		return $contentVersion;
	}

	function approve($target)
	{
		global $user;
		
		$tx = new DataTransaction();
		try 
		{
			$this->joinTransaction($tx);
			$target->joinTransaction($tx);
			$this->populateTo($target);
			$target->save();
			$this->approved=1;
			$this->approved_by = $user->getPrimaryKeyValue();
			$this->approved_date = now();
			$this->save();
			$tx->commit();
		} 
		catch (Exception $e) 
		{
			$tx->rollback();
			throw $e;
		}
	}
	
	/**
	 * Update the versioned fields on the target object to the specified version.
	 * @param DataItem $target the target content object
	 * @param int $version the target version number 
	 * @throws FakoliException
	 */
	static function loadVersion($target, $version)
	{
		checkNumeric($version);
		try
		{
			$contentVersion = Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i AND version_number=:v")
								   ->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue(), ":v", $version)
								   ->executeSingle();
			
			$contentVersion->populateTo($target);
		}
		catch(DataNotFoundException $e)
		{
			throw new FakoliException("Version $version not found");
		}
		
	}
	
	private function populateFrom($target)
	{
		$representation = array();
		foreach($target->versioned_fields as $field)
		{
			$representation[$field] = $target->get($field);
		}
		
		$this->content = json_encode($representation);
	}

	private function populateTo($target)
	{
		$representation = json_decode($this->content, true);
		foreach($target->versioned_fields as $field)
		{
			$target->set($field, $representation[$field]);
		}
	}
		
	private static function nextVersion($target, $tx)
	{
		$version = Query::create('ContentVersion', "WHERE content_class=:c AND content_id=:i")
						->bind(":c", get_class($target), ":i", $target->getPrimaryKeyValue())
						->executeValue("MAX(version_number)");
		
		return intval($version) + 1;
	}
	
	function LastModifiedBy()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->last_modified_by);
	}
	
	function ApprovedBy()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->approved_by);
	}
}

/**@}*/?>

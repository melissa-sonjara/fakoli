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

Fakoli::using("fileshare");

class DocumentLibrary extends DataItem
{
	// Fields
	
	var $fields = array("document_library_id"	=>	Number,
						"name"					=>	String,
						"identifier"			=>	String,
						"description"			=>	Text,
						"hidden"				=>	Boolean,
						"owner_id"				=>	Number,
						"allow_access"			=>	String,
						"manage_folders"		=>	String,
						"upload_files"			=>	String,
						"delete_files"			=>	String,
						"create_date"			=>	Timestamp,
						"last_modified"			=>	Timestamp);

	var $relations = array(	"Owner"		=> "",
							"Documents"	=>	Document,
							"Folders"	=>	DocumentFolder,
							"FileshareUsers"	=>	FileshareUserXref	
	
	);
							
	
	function Owner()
	{
		if(!$this->owner_id)
			return null;
			
		$mgr = new UserManager();
		return $mgr->getUser($this->owner_id);
	}
	
	function isOwner()
	{
		global $user;
		
		if(!$user)
			return false;
			
		$owner = $this->Owner();

		return ($owner->user_id == $user->user_id) ? true : false;
	}
	
	
	function Documents($constraint = "")
	{
		return $this->getRelatedList(Document, "", $constraint);
	}
	
	function Folders($constraint = "")
	{
		return $this->getRelatedList(DocumentFolder, "", $constraint);
	}
		
	function getOwnerName()
	{
		$owner = $this->Owner();
		
		return ($owner) ? $owner->getFullName() : "";
	}
	
	function getLibraryDirectory()
	{
		global $config;
		
		$d = $config['uploadbase'] . DIRECTORY_SEPARATOR . "library_{$this->document_library_id}";
		
		if (!is_dir($d) && !file_exists($d))
		{
			$docs = $d.DIRECTORY_SEPARATOR."documents";
			mkdir($d);
			mkdir($docs);
			chmod($docs, 0777);
		}		
		
		return $d;
	}
	
	function getFolders($constraint = "")
	{
		return reindexList($this->Folders($constraint), "path");
	}
			
	function countDocuments()
	{
		return queryValue(Document, "COUNT(1)", "WHERE document_library_id={$this->document_library_id}");
	}

	static function findByIdentifier($identifier)
	{
		$match = Query::create(DocumentLibrary, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
	}
	
	function FileshareUsers($constraint = "")
	{
		return $this->getRelatedList(FileshareUserXref, "document_library_id", $constraint);	
	}
	
	function getFileshareUser($user_id)
	{
		$xrefs = $this->FileshareUsers("WHERE user_id = $user_id");
		return (count($xrefs) > 0) ? $xrefs[0] : null;	
	}
	
	/*
	 * Check fileshare user-level access
	 */
	function allowAccess()
	{
		global $user;
		
		if(!$user)
			$user = FileshareManager::setVisitor();
			
		if($this->isOwner() || checkRole($this->allow_access) || $this->isFileshareMember($user->user_id))
		{
			return true;
		}
		return false;
	}
	
	/**
	 * Can only delete an empty library.
	 * 
	 * @return boolean
	 */
	function allowDelete()
	{
		if(!$this->document_library_id) return false;
		
		return (!count($this->Documents()) && checkRole($this->delete_files)) ? true : false;
	}
	
	function allowManageFiles()
	{
		global $user;
		
		if(!$user)
			return false;
			
		if($this->isOwner() || checkRole($this->manage_folders) || $this->isFileshareMember())
		{
			return true;
		}
		return false;
		
	}
		
	function fileshareAllowUpload()
	{
		global $user;
		
		if(!$user)
			return false;
		
		if($this->isOwner() || checkRole($this->upload_files) || $this->isFileshareMember())
			return true;
		else
			return false;
	}
	
		
	function isFileshareMember()
	{
		global $user;
		
		$member = $this->getFileshareUser($user->user_id);
		
		return ($member) ? true : false;
	}

	
	/*
	 * Set admin role as default for full access rights to
	 * new libraries. If creating a library in the fileshare
	 * space, also allow "fileshare" role full
	 * access rights.
	 */
	static function getDefaultRole($fileshare = false)
	{
		$identifier = $_REQUEST["identifier"];

		if(preg_match("/fileshare/i", $identifier) || $fileshare)
			$constraint = "WHERE role like 'admin%' or role like 'fileshare%'";
		else
			$constraint = "WHERE role like 'admin%'";
	
		$roles = Query::create(SiteRole, $constraint)->execute();

		if(count($roles) > 0)
			$role = formatItems($roles, "{role}", ",");
			
		return $role;
	}
	
	function getFileSize($document)
	{
		$path = $this->getLibraryDirectory() . DIRECTORY_SEPARATOR . $document->file;
		
		if (file_exists($path))
		{
			return getScaledSize(filesize($path));	
		}
		
		return "";
	}
	
	function DocumentLibrary()
	{
		$this->primary_key = "document_library_id";
		$this->table = "document_library";
	
		$this->default_format = "{name}";
		$this->cacheLocal = true;
		
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 
		
		$this->DataItem(func_get_args());
	}
}
?>
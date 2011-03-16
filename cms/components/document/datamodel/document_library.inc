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

class DocumentLibrary extends DataItem
{
	// Fields
	
	var $fields = array("document_library_id"	=>	Number,
						"name"					=>	String,
						"identifier"			=>	String,
						"description"			=>	Text,
						"owner_id"				=>	Number,
						"allow_access"			=>	String,
						"manage_folders"		=>	String,
						"upload_files"			=>	String,
						"delete_files"			=>	String,
						"last_modified"			=>	Timestamp);

	var $relations = array(	"Owner"		=> "",
							"Documents"	=>	Document,
							"Folders"	=>	DocumentFolder);
							
	
	function Owner()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->owner_id);
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
		return $this->Owner()->getFullName();
	}
	
	function getLibraryDirectory()
	{
		global $config;
		
		return $config['uploadbase'] . DIRECTORY_SEPARATOR . "library_{$this->document_library_id}";
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
	
	static function getDefaultRole()
	{ 	
		$roles = Query::create(SiteRole, "WHERE role like 'admin%'")->execute();
				
		return (count($roles) > 0) ? $roles[0]->role : null;
	}
	
	function DocumentLibrary()
	{
		$this->primary_key = "document_library_id";
		$this->table = "document_library";
	
		$this->default_format = "{name}";
	
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 
		
		$this->DataItem(func_get_args());
	}
}
?>
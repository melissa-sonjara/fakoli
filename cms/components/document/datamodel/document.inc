<?
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

/**
 * This class maps the publication table.
 */
class Document extends DataItem
{
	// Fields
	
	var $fields = array("document_id"			=>	Number,
						"title"					=>	String,
						"file"					=>	String,
						"publication_date"		=>	Date,
						"keywords"				=>  String,
						"notes"					=>	Text,
						"public"				=>	Boolean,
						"last_modified"			=>	Timestamp,
						"document_library_id"	=>	Number,
						"owner_id"				=>	Number
	);
						
	// Relations
	
	var $relations = array(	"Topics"			=>	Topic,
							"Owner"				=>	"",
							"DocumentLibrary"	=>	DocumentLibrary
	);
	
	function Topics($constraint = "")
	{
		return $this->crossReference(Topic, DocumentTopicXref, $constraint);
	}

	function Owner()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->owner_id);
	}
	
	function isOwner()
	{
		global $user;
		$owner = $this->Owner();

		return ($owner->user_id == $user->user_id) ? true : false;
	}
	
	/*
	 * For fileshare space, role-based permissions must be anything
	 * in addition or other than just "allow_access" to get edit
	 * details rights to a document.
	 */
	function fileshareAllowEdit()
	{
		global $user;
		
		$library = $this->DocumentLibrary();
		
		$member = $library->getFileshareUser($user->user_id);
		
		if($this->isOwner() || checkRole($library->manage_folders) || 
		checkRole($library->upload_files) || checkRole($library->delete_files) || $member)
			return true;
		else
			return false;
	}
	
	function fileshareAllowDelete()
	{
		$library = $this->DocumentLibrary();
		
		if($this->isOwner() || $library->isOwner() || 
			checkRole($library->delete_files))
			return true;
		else
			return false;
	}

	function DocumentLibrary()
	{
		return $this->getRelated(DocumentLibrary);
	}
	
	function Document()
	{
		$this->primary_key = "document_id";
		$this->table = "document";
		
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 
		
		$this->default_format = "{title}";
		
		$this->DataItem(func_get_args());
	}
}

?>
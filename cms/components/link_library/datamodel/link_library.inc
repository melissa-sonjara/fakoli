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


class LinkLibrary extends DataItem
{
	var $table = "link_library";
	var $primary_key = "link_library_id";
	var $default_format = "{name}";
	
	// Fields
	
	var $fields = array("link_library_id"	=>	Number,
						"name"				=>	String,
						"identifier"		=>	String,
						"description"		=>	HTML,
						"hidden"			=>	Boolean,
						"owner_id"			=>	Number,
						"read_access"		=>	String,
						"write_access"		=>	String,
						"create_date"		=>	DateTime,
						"last_modified"		=>	Timestamp,
						"enabled"			=>	Boolean);

	var $relations = array(	"Owner"		=> "",
							"Links"	=>	LinkRecord);
							
	
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
	
	
	function Links($constraint = "")
	{
		return $this->getRelatedList(LinkRecord, "", $constraint);
	}

	function getOwnerName()
	{
		$owner = $this->Owner();
		
		return ($owner) ? $owner->getFullName() : "";
	}
			
	function countLinks($published = false)
	{
		$constraint = $published ? " AND published=1" : "";
		
		return queryValue(LinkRecord, "COUNT(1)", "WHERE link_library_id={$this->link_library_id}$constraint");
	}

	static function findByIdentifier($identifier)
	{
		$match = Query::create(LinkLibrary, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
	}
	
	/*
	 * Check user-level access
	 */
	function allowReadAccess()
	{
		global $user;
		
		if(!$user)
			$user = FileshareManager::setVisitor();
			
		if($this->isOwner() || checkRole($this->read_access))
		{
			return true;
		}
		return false;
	}
	
	function allowWriteAccess()
	{
		global $user;
		
		if(!$user)
			return false;
			
		if($this->isOwner() || checkRole($this->write_access))
		{
			return true;
		}
		return false;
		
	}
}
?>
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
 
class VideoGallery extends DataItem
{
	// Fields
	
	var $fields = array("video_gallery_id"	=>	Number,
						"gallery_name"	=>	String,
						"identifier"	=>	String,
						"description"	=>	HTML,
						"keywords"		=>	String,
						"hidden"		=>	Boolean,
						"owner_id"		=>	Number,
						"read_access"	=>	String,
						"write_access"	=>	String,
						"published"		=>	Boolean,
						"last_modified"	=>	Timestamp);
	
	// Relations
	
	var $relations = array(	"Videos"	=>	Video,
							"Owner"		=>	"");

	
	function Videos($constraint = "")
	{
		return $this->getRelatedList(Video, "", $constraint);
	}
	
	function Owner()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->owner_id);
	}
		
	function getOwnerName()
	{
		return $this->Owner()->getFullName();
	}
	
	function countVideos($constraint = "")
	{
		if ($constraint)
		{
			$constraint .= " AND ";
		}
		else
		{
			$constraint = "WHERE ";
		}
		$constraint .= "video_gallery_id={$this->video_gallery_id}";
		return queryValue(Video, "COUNT(1)", $constraint);
	}
	
	function getGalleryDirectory()
	{
		return VideoManager::getVideoFolder() . DIRECTORY_SEPARATOR . "gallery_{$this->video_gallery_id}";
	}
	
	static function findByIdentifier($identifier)
	{
		$match = Query::create(VideoGallery, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
	}
	
	function VideoGallery()
	{
		$this->primary_key = "video_gallery_id";
		$this->table = "video_gallery";
		
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 
		
		$this->default_format = "{gallery_name}";
		
		$this->DataItem(func_get_args());
	}
}

?>
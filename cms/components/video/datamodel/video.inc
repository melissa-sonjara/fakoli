<?php
/**\addtogroup video
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
 
Fakoli::using("image");

class Video extends DataItem
{
	// Fields

	var $fields = array("video_id"			=>	Number,
						"title"				=>	String,
						"description"		=>	HTML,
						"video_file"		=>	String,
						"width"				=>	Number,
						"height"			=>	Number,
						"image_file"		=>	String,
						"user_id"			=>	Number,
						"last_modified"		=>	Timestamp,
						"sort_order"		=>	Number,
						"video_gallery_id"	=>	Number);
	
	// Relationships
	
	var $relations = array("User"			=>	"",
						   "VideoGallery"	=>	VideoGallery);
	
	function User()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
	
	function VideoGallery()
	{
		return $this->getRelated(VideoGallery);
	}
	
	function getFormat()
	{
		$extension = substr($this->video_file, strrpos($this->video_file, ".") + 1);
		return strtoupper($extension);
	}
	
	function Video()
	{
		$this->table = "video";
		$this->primary_key = "video_id";
		
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass(); 		
		
		$this->DataItem(func_get_args());
	}
}

/**@}*/?>
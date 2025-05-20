<?php
/**\addtogroup image
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
 
class ImageGallery extends DataItem
{
	// Fields
	
	var $fields = array("gallery_id"			=>	Number,
						"gallery_name"			=>	String,
						"identifier"			=>	String,
						"description"			=>	HTML,
						"keywords"				=>	String,
						"thumbnail_size" 		=>  Number,
						"thumbnail_format"		=>	String,
					  	"image_link_mode"		=>	String,
						"fix_orientation"		=>	Boolean,
						"randomize_filenames"	=>	Boolean,
						"hidden"				=>	Boolean,
						"owner_id"				=>	Number,
						"read_access"			=>	String,
						"write_access"			=>	String,
						"published"				=>	Boolean,
						"last_modified"			=>	Timestamp);
	
	// Relations
	
	var $relations = array(	"Images"	=>	ImageRecord,
							"Owner"		=>	"");

	static $imageLinkModes = array("None"		=>	"No Resource Link",
								   "Thumbnail"	=>	"Link from Thumbnail",
								   "Lightbox"	=>	"Link from Lightbox");
	
	static $thumbnailFormats = array(""	   => "Use Default",
									 "png" => "PNG",
									 "jpg" => "JPEG");
	
	function Images($constraint = "")
	{
		return $this->getRelatedList(ImageRecord, "", $constraint);
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
	
	function getThumbnailFormat()
	{
		return ($this->thumbnail_format) ? $this->thumbnail_format : Settings::getValue("image", "thumbnail_format");
	}
				
	function countImages($constraint = "")
	{
		if ($constraint)
		{
			$constraint .= " AND ";
		}
		else
		{
			$constraint = "WHERE ";
		}
		$constraint .= "gallery_id={$this->gallery_id}";
		return queryValue(ImageRecord, "COUNT(1)", $constraint);
	}
	
	function getGalleryDirectory()
	{
		global $config;
		
		return $config['uploadbase'] . DIRECTORY_SEPARATOR . "gallery_{$this->gallery_id}";
	}
	
	static function findByIdentifier($identifier)
	{
		$match = Query::create(ImageGallery, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
	}
	
	function __construct()
	{
		$this->primary_key = "gallery_id";
		$this->table = "image_gallery";
		
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 
		
		$this->default_format = "{gallery_name}";
		
		parent::__construct(func_get_args());
	}
}

/**@}*/?>

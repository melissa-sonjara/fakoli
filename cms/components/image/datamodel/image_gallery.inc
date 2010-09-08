<?php

class ImageGallery extends DataItem
{
	// Fields
	
	var $fields = array("gallery_id"	=>	Number,
						"gallery_name"	=>	String,
						"identifier"	=>	String,
						"description"	=>	HTML,
						"keywords"		=>	String,
						"owner_id"		=>	Number,
						"read_access"	=>	String,
						"write_access"	=>	String,
						"last_modified"	=>	Timestamp);
	
	// Relations
	
	var $relations = array(	"Images"	=>	Image,
							"Owner"		=>	"");

	
	function Images($constraint = "")
	{
		return $this->getRelatedList(Image, "", $constraint);
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
	
	function countImages()
	{
		return queryValue(Image, "COUNT(1)", "WHERE gallery_id={$this->gallery_id}");
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
	
	function ImageGallery()
	{
		$this->primary_key = "gallery_id";
		$this->table = "image_gallery";
		
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 
		
		$this->default_format = "{gallery_name}";
		
		$this->DataItem(func_get_args());
	}
}

?>
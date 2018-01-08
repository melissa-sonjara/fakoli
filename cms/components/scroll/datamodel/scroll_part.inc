<?php
class ScrollPart extends DataItem
{
	var $table = "scroll_part";
	var $primary_key = "scroll_part_id";
	
	var $fields = array("scroll_part_id"	=>	Number,
						"scroll_id"			=>	Number,
						"title"				=>	String,
						"content_type"		=> 	String,
						"content"			=>	HTML,
						"php_code_file"		=>	String,
						"css_class"			=>	String,
						"styles"			=>	String,
						"image_id"			=>	Image,
						"sort_order"		=>	Number);
						
	var $relations = array(	"Scroll"			=>	Scroll,
							"BackgroundImage"	=>	ImageRecord
	);
	
	var $field_aliases = array(	"css_class"	=>	"CSS Class",
								"styles"	=>	"Inline CSS Styles",
								"image_id"	=>	"Background Image");
	function Scroll()
	{
		return $this->getRelated(Scroll);
	}
	
	function BackgroundImage()
	{
		return $this->getRelated(ImageRecord);
	}
}
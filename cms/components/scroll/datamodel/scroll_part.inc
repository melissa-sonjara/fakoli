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
						"sort_order"		=>	Number);
						
	var $relations = array("Scroll"	=>	Scroll);
	
	function Scroll()
	{
		return $this->getRelated(Scroll);
	}
}
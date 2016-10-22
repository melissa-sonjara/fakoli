<?php
class Scroll extends DataItem
{
	var $table = "scroll";
	var $primary_key = "scroll_id";
	
	var $fields = array("scroll_id"					=>	Number,
						"identifier"				=>	String,
						"title"						=>	String,
						"meta_tag_description"		=>	Text,
						"meta_tag_keyword"			=>	String,
						"site_id"					=>  Number,
						"show_internal_navigation"	=>	Boolean,
						"exclude_from_search"		=>	Boolean,
						"created_date" 				=>	Date,
						"edited_date"				=>	Timestamp);
	
	var $relations = array(	"Parts"	=>	ScrollPart,
							"Site"	=>	Site);
	
	function Parts($constraint = "ORDER BY sort_order")
	{
		return $this->getRelatedList(ScrollPart, "", $constraint);
	}
	
	function Site()
	{
		return $this->getRelated(Site);
	}
	
	function countParts()
	{
		return Query::create(ScrollPart, "WHERE scroll_id=:s")->bind(":s", $this->scroll_id)->count();
	}
	
	static function findByIdentifier($identifier)
	{
		$match = Query::create(Scroll, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
	}	
}
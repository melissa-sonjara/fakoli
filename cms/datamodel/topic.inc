<?
/* =======================================================================
 * NCUIH Online Toolkit
 * Developed by Sonjara, Inc. for NCUIH
 * For further information please contact Andy Green at andy@sonjara.com
 * =======================================================================
 */
 
require_once realpath(dirname(__FILE__)."/../../framework/data_item.inc");

/**
 * This class maps the topic table.
 */
class Topic extends DataItem
{
	// Fields
	
	var $fields = array("topic_id"	=>	Number,
						"topic"		=>	String
						);
						
	// Relations
	
	var $relations = array(	"Publications"	=>	Publication);
							
	function Publications($constraints = "")
	{
		return $this->crossReference(Publication, PublicationTopicXref, $constraints);
	}
		
	function Topic()
	{
		$this->primary_key = "topic_id";
		$this->table = "topic";
		
		$this->default_format = "{topic}";
		
		$this->DataItem(func_get_args());
	}
}

class PublicationTopicXref extends DataItem
{
	// Fields
	var $fields = array("publication_topic_xref_id"	=>	Number,
						"publication_id"			=>	Number,
						"topic_id"					=>	Number);
						
	// Relations
	
	var $relations = array(	"Publication"	=>	Publication,
							"Topic"			=>	Topic);
							
	function Publication()
	{
		return $this->getRelated(Publication);
	}
	
	function Topic()
	{
		return $this->getRelated(Topic);
	}
	
	function PublicationTopicXref()
	{
		$this->primary_key = "publication_topic_xref_id";
		$this->table = "publication_topic_xref";
		
		$this->DataItem(func_get_args());
	}
}

class GroupTopicXref extends DataItem
{
	// Fields
	var $fields = array("group_topic_id"	=>	Number,
						"group_id"			=>	Number,
						"topic_id"			=>	Number);
						
	// Relations
	
	var $relations = array(	"Group"	=>	Group,
							"Topic"	=>	Topic);
							
	function Grp()
	{
		return $this->getRelated(Group);
	}
	
	function Topic()
	{
		return $this->getRelated(Topic);
	}
	
	function GroupTopicXref()
	{
		$this->primary_key = "group_topic_id";
		$this->table = "grp_topic_xref";
		
		$this->DataItem(func_get_args());
	}
}
?>
<?php

class BindingTarget extends DataItem
{
	var $table = "binding_target";
	var $primary_key = "binding_target_id";
	
	var $fields = array("binding_target_id"		=>	Number,
						"target_class"			=>	String,
						"associated_classes"	=>	String,
						"enable_facet_filter"	=>	Boolean);	
	
	function getBoundItemIds($item)
	{
		$cl = get_class($item);
	
		if (!isset($this->_termCache[$cl]))
		{
			$this->_termCache[$cl] = IndexedQuery::create(BindingSummary, "WHERE target_class=:t and class=:c", "id")
												 ->bind(":t", $this->target_class,
													    ":c", get_class($item))
												 ->execute();
		}
	
		return $this->_termCache[$cl][$item->get($item->getPrimaryKey())]->target_ids;
	}
}

class BindingRecord extends DataItem
{
	var $table = "binding";
	var $primary_key = "binding_id";
	
	var $fields = array("binding_id"	=>	Number,
						"target_class"	=>	String,
						"target_id"		=>	Number,
						"class"			=>	String,
						"id"			=>	Number);
}

class BindingSummary extends DataItem
{
	var $table = "binding_summary";
	
	var $fields = array("class" 		=>	String,
						"id"			=>	Number,
						"target_class"	=>	String,
						"target_ids"	=>	String);
}	
?>
<?php
/**\addtogroup connectable
 * @{
 */


class ConnectionRecord extends DataItem
{
	var $table = "connection_record";
	var $primary_key = "connection_id";
	
	var $fields = array("connection_id"	=>	'Number',
						"source_class"	=>	'String',
						"source_id"		=>	'Number',
						"target_class"	=>	'String',
						"target_id"		=>	'Number');
	
	function Source()
	{
		return $this->getRelated($this->source_class, "source_id");
	}
	
	function Target()
	{
		return $this->getRelated($this->target_class, "target_id");
	}
}
						
/**@}*/?>

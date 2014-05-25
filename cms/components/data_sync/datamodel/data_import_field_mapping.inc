<?php
class DataImportFieldMapping extends DataItem
{
	var $primary_key = "mapping_id";
	var $table = "data_import_field_mapping";
	
	var $columns = null;
	
	var $fields = array("mapping_id"		=>	Number,
			            "class"				=>	String,
						"client_field"		=>	String,
						"import_column"		=>	String,
						"matching"			=>	Boolean,
						"required"			=>	Boolean,
						"grouping"			=>	Boolean,
						"import_profile_id"	=>	String);
	
	function ImportProfile()
	{
		return $this->getRelated(DataImportProfile);
	}
	
	function getColumns()
	{
		if ($this->columns)
		{
			return $this->columns;
		}
		
		$this->columns = explode(",", $this->import_column);
		return $this->columns;
	}
}
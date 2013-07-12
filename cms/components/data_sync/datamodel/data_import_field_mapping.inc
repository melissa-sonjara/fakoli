<?php
class DataImportFieldMapping extends DataItem
{
	var $primary_key = "mapping_id";
	var $table = "data_import_field_mapping";
	
	var $fields = array("mapping_id"	=>	Number,
			            "class"			=>	String,
						"client_field"	=>	String,
						"import_column"	=>	String,
						"matching"		=>	Boolean,
						"required"		=>	Boolean);
	
}
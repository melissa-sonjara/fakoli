<?php
/**\addtogroup section
 * @{
 */



/**
 * DataItem pseudo-object to handle section XML imports.
 * 
 * @author andy
 *
 */
class SectionImportFile extends DataItem
{
	// Fields
	var $fields = array("import_file_id"	=>	Number,
						"import_file"	=>	String);
	
	function SectionImportFile()
	{
		$this->table = "";
		$this->primary_key = "import_file_id";
		
		$this->DataItem(func_get_args());
	}
}

/**@}*/?>
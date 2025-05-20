<?php
/**\addtogroup data_sync
 * @{
 */


class DataImportProfile extends DataItem
{
	var $table = "data_import_profile";
	var $primary_key = "import_profile_id";
	
	var $fields = array("import_profile_id"	=>	'Number',
						"profile"			=>	'String',
						"class"				=>	'String',
						"notes"				=>	'Text');
	
}

/**@}*/?>

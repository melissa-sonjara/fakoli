<?php
/**\addtogroup document
 * @{
 */


class DocumentMove extends DataItem
{
	var $primary_key = "move_id";
	
	var $fields = array("move_id"		=>	Number,
					    "from"			=>	Number,
						"to"			=>	Number);
}
/**@}*/?>
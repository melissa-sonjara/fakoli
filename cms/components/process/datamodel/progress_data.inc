<?php
/**\addtogroup process
 * @{
 */



class ProgressData extends DataItem
{
	// Fields
	var $fields = array("status"		=>	'String',
						"message"		=>	'String',
						"percentage"	=>	'Number');
	
	function __construct()
	{
		parent::__construct(func_get_args());
	}
}

/**@}*/?>

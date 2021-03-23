<?php
/**\addtogroup process
 * @{
 */



class ProgressData extends DataItem
{
	// Fields
	var $fields = array("status"		=>	String,
						"message"		=>	String,
						"percentage"	=>	Number);
	
	function ProgressData()
	{
		$this->DataItem(func_get_args());
	}
}

/**@}*/?>
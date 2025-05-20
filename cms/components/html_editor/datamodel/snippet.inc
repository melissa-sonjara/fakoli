<?php
/**\addtogroup html_editor
 * @{
 */



class Snippet extends DataItem
{
	var $primary_key = "snippet_id";
	var $table = "snippet";
	
	var $fields = array("snippet_id"	=>	'Number',
						"snippet_name"	=>	'String',
						"description"	=>	'Text',
						"snippet"		=>	'HTML',
						"user_id"		=>	'Number',
						"shared"		=>	'Boolean',
						"last_modified"	=>	'Timestamp');
	
	function User()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
}
/**@}*/?>

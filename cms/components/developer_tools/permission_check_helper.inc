<?php
/**\addtogroup developer_tools
 * @{
 */


Fakoli::using("developer_tools");

class PermissionsCheckHelper
{
	var $subject;
	
	function __construct($subject)
	{
		$this->subject = $subject;
	}

	function checkPermissions($content)
	{
		$section = $content->Section();
		$permissions = $section->getPermissions($content);
		if ($permissions)
		{
			if (!Settings::checkPermissions($permissions, $this->subject))
			{
				return false;
			}
		}
		else if (!checkRole($section->getRole($content), $this->subject))
		{
			return false;
		}
		
		return true;
	}
	
	function onStartRow($content)
	{
		$canAccess = $this->checkPermissions($content);
	
		return $canAccess ? "yay" : "nay";
	}
	
	function formatAccess($content)
	{
		$canAccess = $this->checkPermissions($content);
	
		return $canAccess ? "<img src='/fakoli/images/on.png' alt='Yes'/>" : "<img src='/fakoli/images/off.png' alt='No'/>";
	}
}
/**@}*/?>
<?php


class MenuManager
{
	function MenuManager()
	{
		
	}
	
	static function deleteRole($del_role)
	{
		trace("MenuManager: del_role $del_role", 3);
		$constraint = "WHERE role like '%$del_role%'";
		
		$mItems = Query::create(MenuItem, $constraint)->execute();
		RoleManager::deleteRoleFromString($mItems, $del_role);
		
		return $del_role;
	}		
}
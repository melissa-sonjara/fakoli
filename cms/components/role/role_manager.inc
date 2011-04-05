<?php

Fakoli::using("component");

class RoleManager
{
	function RoleManager()
	{
		
	}
	
	/*
	 * Called by ComponentManager, PageManager, UserManager, and
	 * MenuItemManager
	 */
	static function deleteRoleFromString(&$items, $del_role, $field = "")
	{
		if(count($items) == 0)
			return;
			
		if(!$field)
			$field = "role";

		foreach($items as $item)
		{
			if(!$item->$field)
				continue;
				
			$newRoles = array();
			
			$roles = explode(",", $item->$field);
			foreach($roles as $role)
			{
				if(!preg_match("/$del_role/", $role))
					$newRoles[] = $role;
			}	
				
			if(count($newRoles) > 0)
				$newRole = implode(",", $newRoles);
			else
				$newRole = "";
			
			$item->$field = $newRole;
			$item->filter = new InclusionFilter($field);
			$item->save();
		}
	}
}
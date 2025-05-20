<?php
/**\addtogroup role
 * @{
 */


/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::using("component");

class RoleManager
{
	function __construct()
	{
		
	}
	
	/**
	 * @deprecated
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
	
	/**
	 * Adds the specified role to the access list field in each of the supplied DataItems.
	 * 
	 * @param mixed $items the DataItem or array of DataItems to be updated
	 * @param string $role the role to add to the access list field
	 * @param string $field the name of the field (default is 'role')
	 */
	static function addRoleToAccessList($items, $role,  $field = "role")
	{
		if (count($items) == 0) return;
		if (is_object($items))
		{
			if (!$items instanceof DataItem) return;
			$items = array( $items );
		}
		
		foreach($items as $item)
		{
			if (!$item->hasField($field)) continue;
						
			$roles = explode(",", $item->get($field));
			$found = false;
			foreach($roles as $r)
			{
				if ($r == $role)
				{
					$found = true;
					break;
				}
			}
			
			if (!$found)
			{
				$roles[] = $role;
				$oldFilter = $item->filter;
				$item->filter = new InclusionFilter($field);
				$item->set($field, implode(",", $roles));
				$item->save();
				$item->filter = $oldFilter;
			}
		}
	}
	
	/**
	 * Removes the specified role from the access list field in each of the supplied DataItems
	 * 
	 * @param mixed $items the DataItem or array of DataItems to be updated
	 * @param string $role the role to add to the access list field
	 * @param string $field the name of the field (default is 'role')
	 */
	static function removeRoleFromAccessList($items, $role, $field = "role")
	{
		if (count($items) == 0) return;
		if (is_object($items))
		{
			if (!$items instanceof DataItem) return;
			$items = array( $items );
		}
		
		foreach($items as $item)
		{
			if (!$item->hasField($field)) continue;
			
			$roles = explode(",", $item->get($field));
			$newRoles =array();
			$found = false;
			foreach($roles as $r)
			{
				if ($r == $role)
				{
					$found = true;
				}
				else
				{
					$newRoles[] = $r;
				}
			}
			
			if ($found)
			{
				$oldFilter = $item->filter;
				$item->filter = new InclusionFilter($field);
				$item->set($field, implode(",", $newRoles));
				$item->save();
				$item->filter = $oldFilter;
			}
		}
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("role", "Roles", new SimpleSerializationHandler(SiteRole));
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new RoleUpgradeManager();
		$mgr->upgrade($version);
	}	
}
/**@}*/?>

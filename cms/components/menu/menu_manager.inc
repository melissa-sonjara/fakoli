<?php
/**\addtogroup menu
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

class MenuSerializationHandler
{
	function MenuSerializationHandler()
	{
	}
	
	function export()
	{
		$xml  = "\n<MenuMap>";
		$xml .= SerializationManager::serialize(Menu, "ORDER BY menu_id");
		$xml .= SerializationManager::serialize(MenuItem, "ORDER BY menu_item_id");
		$xml .= "</MenuMap>";
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(Menu, $doc, $tx);
		SerializationManager::unserialize(MenuItem, $doc, $tx);		
	}	
}

class MenuManager
{
	static $displayFlags = array();
	
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

	static function setDisplayFlag($name, $value = true)
	{
		MenuManager::$displayFlags[$name] = $value;
	}
	
	static function testFlags($flags)
	{
		if (!$flags) return true;
		$flags = explode(",", $flags);
		
		foreach($flags as $flag)
		{
			if (startsWith($flag, "!"))
			{
				$flag = substr($flag, 1);
				if (MenuManager::$displayFlags[$flag] == true) return false;
			}
			else if (!MenuManager::$displayFlags[$flag]) return false;
		}
		
		return true;
	}
	static function upgradeComponent($version)
	{
		$mgr = new MenuUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("menu", "Menus and Menu Items", new MenuSerializationHandler());
		return true;
	}
}
/**@}*/?>
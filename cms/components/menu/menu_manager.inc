<?php
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
		$menus		= Query::create(Menu, "ORDER BY menu_id")->execute();
		$menuItems  = Query::create(MenuItem, "ORDER BY menu_item_id")->execute();

		$xml  = "\n<MenuMap>";
		$xml .= toXML("MenuList", $menus, "");
		$xml .= toXML("MenuItemList", $menuItems, "");
		$xml .= "</MenuMap>";
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		trace("== Importing Menu Map", 3);
		
		$menuNodes = $doc->getElementsByTagName("Menu");
	
		$menus = array();
		
		foreach($menuNodes as $node)
		{
			$menu = new Menu();
			$menu->fromXML($node);
			$menus[] = $menu;
		}
		
		$menuItemNodes = $doc->getElementsByTagName("MenuItem");
		
		$menuItem = array();
		
		foreach($menuItemNodes as $node)
		{
			$item = new MenuItem();
			$item->fromXML($node);
			$menuItem[] = $item;
		}
		
		$m = new Menu();
		$m->joinTransaction();
		$m->deleteAll();
		$m->disablePrimaryKey();
		
		foreach($menus as $menu)
		{
			$menu->_disablePK = true;
			$menu->joinTransaction($tx);
			$menu->save();
		}
			
		$s->enablePrimaryKey();
		
		$m = new MenuItem();
		$m->joinTransaction($tx);
		$m->deleteAll();
		$m->disablePrimaryKey();

		foreach($menuItem as $item)
		{
			$item->_disablePK = true;
			$item->joinTransaction($tx);
			$item->save();
		}
		
		$m->enablePrimaryKey();
	}	
}

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

	static function upgradeComponent()
	{
		$mgr = new MenuUpgradeManager();
		$mgr->upgrade();
	}	
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("menu", "Menus and Menu Items", new MenuSerializationHandler());
		return true;
	}
}
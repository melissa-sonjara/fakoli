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


class Menu extends DataItem
{
	// Fields
	var $fields = array("menu_id"					=>	Number,
						"site_id"					=>	Number,
						"name"						=>	String,
						"identifier"				=>	String,
						"description"				=>	Text,
						"css_class"					=>	String,
						"highlight_current_item"	=>	Boolean,
						"highlight_current_section"	=>	Boolean);
	
	// Relations
	var $relations = array(	"Site"		=>	Site,
							"MenuItems"	=>	MenuItem);
	
	function Site()
	{
		return $this->getRelated(Site);
	}

	function MenuItems($constraint = "")
	{
		return $this->getRelatedList(MenuItem, "", $constraint);
	}
	
	// Constructor
	function Menu()
	{
		$this->primary_key = "menu_id";
		$this->table = "menu";
		
		$this->default_format = "{name}";
		$this->pretty_class_name = "Menu";
		$this->DataItem(func_get_args());
	}
}

class MenuItem extends DataItem
{
	// Fields
	var $fields = array("menu_item_id"	=> Number,
						"title"			=> String,
						"custom_format"	=> String,
						"menu_id"		=> Number,
						"parent_id" 	=> Number,
						"identifier"	=> String,
						"page_id"   	=> Number,
						"url"       	=> String,
						"sort_order"	=> Number,
						"role"		 	=> String,
						"permissions"	=> String,
						"published"		=> Boolean,
						"required_parameters"		=>	String);
	
	// Relations
	
	var $relations = array(	"Page"		=> 	Page,
							"Menu"		=>	Menu,
							"Parent"	=>	MenuItem,
							"Children"	=>	MenuItem);
	
	function Page()
	{
		$pages = Query::create(Page, "WHERE identifier=:i")
					->bind(":i", $this->identifier)
					->execute();
		if (count($pages) > 0)
		{
			return $pages[0];
		}

		$pages = Query::create(ComponentPage, "WHERE identifier=:i")
					->bind(":i", $this->identifier)
					->execute();
		if (count($pages) > 0)
		{
			return $pages[0];
		}
		
		return null;
	}
	
	function Menu()
	{
		return $this->getRelated(Menu);
	}
	
	function Parent()
	{
		if (!$this->parent_id) return null;
		
		return $this->getRelated(MenuItem, "parent_id");
	}
	
	function Children($constraint = "ORDER BY sort_order, title")
	{
		return query(MenuItem, "WHERE parent_id={$this->menu_item_id} $constraint");
	}

	// Constructor
	function MenuItem()
	{
		$this->primary_key = "menu_item_id";
		$this->table = "menu_item";
		
		$this->default_format = "{title}";
		$this->pretty_class_name = "Menu Item";
		$this->DataItem(func_get_args());
	}
}
?>
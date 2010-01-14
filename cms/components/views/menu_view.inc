<?php

/**************************************************************

Title: module_view.php

Description: To view the menu.

author Reshma Joshi for Sonjara, Inc.

date: 11/09

***************************************************************/

require_once realpath(dirname(__FILE__))."/../../datamodel/menus.inc";
require_once realpath(dirname(__FILE__))."/../../datamodel/page.inc";
require_once realpath(dirname(__FILE__))."/../../../framework/tree.inc";

class MenuView
{
	var $m;
	var $pages;
	
	function MenuView($m)
	{
		$this->m = $m;
		$this->pages = indexedQuery(Page, "", "page_id", new InclusionFilter("page_id", "identifier", "published", "role"));
		
		//linkItemsInHTMLFields($this->module);
	}
	
	function getLink($menuItem)
	{
		// If the URL field is set explicitly, use that
		if ($menuItem->url) return $menuItem->url;
		
		if ($menuItem->page_id)
		{
			if (array_key_exists($menuItem->page_id, $this->pages))
			{
				$page = $this->pages[$menuItem->page_id];
				if (!$page->published)
				{
					// No link if the page is unpublished, or the user role doesn't match.
					return null;
				}
				
				return ($page->identifier) ? $page->identifier : $page->page_id;
			}
			else
			{
				return null;
			}
		}
		
		// No related page or URL - use a placeholder link.
		return "#";
	}
	
	function drawView($menu_type)
	{   
		// Retrieve pages so we can look up identifiers, publication status and role.
		
		$temps = query(Menus, "WHERE menu_type='$menu_type' AND parent_id=0 ORDER BY sort_order");
		foreach ($temps as $temp) { $parent_id = $temp->menu_id; }
	
		
		$menus = query(Menus, "WHERE menu_type='$menu_type' AND parent_id=$parent_id ORDER BY sort_order");
		$gmenus =	groupedQuery(Menus, "WHERE menu_type='$menu_type' AND parent_id!=0 ORDER BY sort_order", "parent_id");
		
		
		echo '<div class="sidebarmenu">'; // has end tag
		echo '<ul id="sidebarmenu1">'; // has end tag
		foreach($menus as $m) 
		{
		  
		  if ( preg_match('/--*/', $m->title) ) {
    		echo "<li><img src=/images/placeholder_6.jpg></li>";
          }
		  else 
		  {
			$link = $this->getLink($m);
			
			if(!$gmenus[$m->menu_id]) 
			{
				if ($link) echo "<li><a href='$link'>$m->title</a></li>"; // has et
			}
			else 
			{
			     if ($link)
			     {
			     	echo "<li><a href='$link'>$m->title</a>"; // no li et
				    if (array_key_exists($m->menu_id, $gmenus))
				    {
				     	echo "<ul>"; // no ul et
					    foreach ($gmenus[$m->menu_id] as $submenu) 
					    {
					    	$link = $this->getLink($submenu);
					    	
					    	if ($link)
					    	{
				            	echo "<li><a href='$link'>$submenu->title</a>";  // only one level; 
					            if($gmenus[$submenu->menu_id]) 
					            {
					             	echo "<ul>";
					             	foreach ($gmenus[$submenu->menu_id] as $subsubmenu) 
					             	{
					             		$link = $this->getLink($subsubmenu);
					             		if ($link) echo "<li><a href='$link'>$subsubmenu->title</a></li>"; // level 3 menus GOOD
					             	}
					             	echo "</ul>"; // end ul for level 3 //GOOD
					             }
					             else 
					             {
					             	echo "</li>"; //end tag for level 2 good
					             }
					    	}
						}
			     
					    echo "</ul>";
					    echo "</li>";
				    }
				}
			}
		  }			     			     
		} # main foreach
		
		echo "</ul>"; // top level ul
		echo "</div>";
		
	}
}
	
?>
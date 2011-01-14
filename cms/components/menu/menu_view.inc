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

/**************************************************************

Title: module_view.php

Description: To view the menu.

author Reshma Joshi for Sonjara, Inc.

date: 11/09

***************************************************************/

Fakoli::using("page", "component");

class MenuView
{
	var $m;
	var $pages;
	var $componentPages;
	
	function MenuView($m)
	{
		$this->m = $m;

		$this->pages = IndexedQuery::create(Page)
									->constraints("WHERE site_id=:s")
									->indexBy("identifier")
									->filter(new InclusionFilter("page_id", "identifier", "published", "role"))
									->bind(":s", $this->m->site_id)
									->execute();
		
		$this->componentPages = IndexedQuery::create(ComponentPage)
									->indexBy("identifier")
									->execute();
	}
	
	function getLink($menuItem)
	{
		// If the URL field is set explicitly, use that
		if ($menuItem->url) return $menuItem->url;
		
		if ($menuItem->identifier)
		{
			if (array_key_exists($menuItem->identifier, $this->pages))
			{
				$page = $this->pages[$menuItem->identifier];
				if (!$page->published)
				{
					// No link if the page is unpublished, or the user role doesn't match.
					return null;
				}
				
				$link = $menuItem->identifier;
				if ($this->queryString) $link .= $this->queryString;
				return $link;
			}
			else if (array_key_exists($menuItem->identifier, $this->componentPages))
			{
				$page = $this->componentPages[$menuItem->identifier];
				
				// JDG - remove check role on page 12/14/2011
				// Allow menu item link to show even if user can't get to page
				if (!$page->enabled)
				{				
					// No link if the component page is unpublished.
					return null;
				}
				
				$link = $menuItem->identifier;
				if ($this->queryString) $link .= $this->queryString;
				return $link;
			}
			else
			{
				return null;
			}
		}
		
		// No related page or URL - use a placeholder link.
		return "#";
	}
	
	function drawView()
	{   	
		global $script;
		$script .= <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function() { new FakoliMenu('{$this->m->identifier}'); });
</script>
ENDSCRIPT;
		ob_start();
		
		$gmenus = regroupList($this->m->MenuItems("ORDER BY sort_order"), "parent_id");
		
		$css = ($this->m->css_class) ? " class='{$this->m->css_class}'" : "";
		echo "<ul{$css}>"; // has end tag
		foreach($gmenus[0] as $m) 
		{
		  
		  if ( preg_match('/^\\-\\-*/', $m->title) ) 
		  {
    		echo "<li><hr/></li>";
          }
		  else 
		  {
			$link = $this->getLink($m);
			
			if(!$gmenus[$m->menu_item_id]) 
			{
				if ($link) echo "<li><a href='$link'>$m->title</a></li>"; // has et
			}
			else 
			{
			     if ($link)
			     {
			     	echo "<li><a href='$link'>$m->title</a>"; // no li et
				    if (array_key_exists($m->menu_item_id, $gmenus))
				    {
				     	echo "<ul>"; // no ul et
					    foreach ($gmenus[$m->menu_item_id] as $submenu) 
					    {
					    	$link = $this->getLink($submenu);
					    	
					    	if ($link)
					    	{
				            	echo "<li><a href='$link'>$submenu->title</a>";  // only one level; 
					            if($gmenus[$submenu->menu_item_id]) 
					            {
					             	echo "<ul>";
					             	foreach ($gmenus[$submenu->menu_item_id] as $subsubmenu) 
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
		
		$out = ob_get_contents();
		ob_end_clean();
		
		return $out;
	}
}
	
?>
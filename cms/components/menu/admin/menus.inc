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

Fakoli::using("menu");
Fakoli::using("site");

Fakoli::assertRole("admin");

$menu_item = "Menus";

$sites = query(Site, "ORDER BY site_name");

$menusBySite = groupedQuery(Menu, "ORDER BY name", "site_id");

$title = "Menus";



foreach($sites as $site)
{
?>
<h3><?echo $site->site_name?> (<a href="<?echo $site->domain?>" target="_blank"><?echo $site->domain?>)</a></h3>
<?
	if (array_key_exists($site->site_id, $menusBySite))
	{
		$menus = $menusBySite[$site->site_id];
?>
<dl>
<?
		foreach($menus as $menu)
		{
			
?>
	<dt><a href="/admin/menu_form?menu_id=<?echo $menu->menu_id?>"><?echo $menu->name?> (<?echo $menu->identifier?>)</a></dt>
	<dd><?echo $menu->description?></dd>
<?		
		}

?>
</dl>
<?
	}
	else
	{
?>
<p><em>No menus have been defined for this site.</em></p>
<?
	} 
}
?>
<br/>
<a class="button" href="/admin/menu_form">Add a Menu</a>
<?

?>

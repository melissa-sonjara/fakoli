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

Fakoli::using("role");
Fakoli::usingFile("framework/data_view.inc");
Fakoli::assertRole("admin");

$menu_item = "Roles";
$title = "Manage User Roles";

$roles = query(SiteRole, "ORDER BY name");


$table = new DataListView($roles, "roles");

$table->column("Role", "<a href='role_form?role_id={role_id}'>{role}</a>", true, 'width: 20%; vertical-align: top')
	  ->column("Name & Description", "<strong>{name}</strong><br/>{description}", true, 'width: 40%')
	  ->column("Home Page", "{home_page}", true, "width: 40%; vertical-align: top");
$table->sortable = true;
$table->filter = true;
$table->pageSize = 5;
$table->emptyMessage = "No Site User Roles have been defined.";

$script = $table->writeScript();



$table->drawView();
?>
<br/>
<button class="button" onclick="go('/admin/role_form')">Add New User Role</button>

<?

?>
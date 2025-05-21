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

Fakoli::using("user");
Fakoli::usingFile("framework/data_view.inc");

Fakoli::assertRole("admin");

$menu_item = "Users";
$title = "Manage Users";

$all = checkNumeric($_GET['all']);

$constraint = $all ? "ORDER BY role, email" : "WHERE active=1 ORDER BY role, email";

$site_users = query('SiteUser', $constraint);


$table = new DataListView($site_users, "site_users");

$table->column("Name", "<a href='/admin/user_form?user_id={user_id}'>{last_name}, {first_name}</a>", true, 'width: 30%')
	  ->column("Title", "{title}", true, 'width: 30%')
	  ->column("Email Address", "{email}", true, "width: 30%")	 
	  ->column("Role", "{role}", true, "width: 10%");	   
$table->sortable = true;
$table->filter = true;
$table->pageSize = 20;
$table->emptyMessage = "No Site Users have been added.";
$table->excelFile = "Users.xls";

$script = $table->writeScript();


?>
<input type="checkbox" class="checkbox" name="all" value="1"<?if ($all) echo " checked='checked'";?> onclick="go('/admin/users?all=' + (this.checked?'1':'0'))">&nbsp;Show inactive user accounts<br/><br/>
<?
$table->drawView();
?>
<br/>
<button class="button" onclick="go('/admin/user_form')">Add New User</button>

<?

?>

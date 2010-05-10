<?php
require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/data_view.inc";

$menu_item = "Users";
$title = "Manage Users";

$all = checkNumeric($_GET['all']);

$constraint = $all ? "ORDER BY role, email" : "WHERE active=1 ORDER BY role, email";

$site_users = query(SiteUser, $constraint);


$table = new DataListView($site_users, "site_users");

$table->column("Name", "<a href='{getAdminForm()}?site_user_id={site_user_id}'>{last_name} {salutation}. {first_name}</a>", true, 'width: 30%')
	  ->column("Title", "{title}", true, 'width: 30%')
	  ->column("Email Address", "{email}", true, "width: 30%")	 
	  ->column("Role", "{role}", true, "width: 10%");	   
$table->sortable = true;
$table->filter = true;
$table->pageSize = 20;
$table->emptyMessage = "No Site Users have been added.";
$table->excelFile = "Users.xls";

$script = $table->writeScript();

require_once admin_begin_page;
?>
<input type="checkbox" class="checkbox" name="all" value="1"<?if ($all) echo " checked='checked'";?> onclick="go('/cms/admin/users.php?all=' + (this.checked?'1':'0'))">&nbsp;Show inactive user accounts<br/><br/>
<?
$table->drawView();
?>
<br/>
<button onclick="go('user_form.php')">Add New User</button>

<?
require_once admin_end_page;
?>
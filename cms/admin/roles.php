<?php
require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/data_view.inc";

$menu_item = "Roles";
$title = "Manage User Roles";

$roles = query(SiteRole, "ORDER BY name");


$table = new DataListView($roles, "roles");

$table->column("Role", "<a href='role_form.php?role_id={role_id}'>{role}</a>", true, 'width: 30%; vertical-align: top')
	  ->column("Name & Description", "<strong>{name}</strong><p>{description}", true, 'width: 70%');
$table->sortable = true;
$table->filter = true;
$table->pageSize = 20;
$table->emptyMessage = "No Site User Roles have been defined.";

$script = $table->writeScript();

require_once admin_begin_page;

$table->drawView();
?>
<br/>
<button class="button" onclick="go('role_form.php')">Add New User Role</button>

<?
require_once admin_end_page;
?>
<?php
Fakoli::using("developer_tools", "section");
Fakoli::usingFeature("grouped_data_view");

$page_title = "Permission Map";
$menu_item = "Permission Map";

$styles .= <<<ENDSTYLES

<style>
 tr.yay { background-color: #eeffee }
 tr.nay { background-color: #ffeeee }
</style>

ENDSTYLES;

$site_id = checkNumeric($_GET["site_id"]);

$sites = query('Site', "ORDER BY site_name");

if (!$site_id) $site_id = $sites[0]->site_id;

$userMgr = new UserManager();
$userClass = $userMgr->getUserClass();
$subject = new $userClass;

$userPK = $subject->getPrimaryKey();
$user_id = checkNumeric($_GET[$userPK]);
$usernameField = $userMgr->getUsernameField();

if (!$user_id) $user_id = $user->getPrimaryKeyValue();

$subject->load($user_id);

$tabs = new DataItemTabBar("tabs", $sites);

$permissions = Query::create('Settings', "WHERE options='Permissions' ORDER BY component")->execute();

$userPermissions = array();
foreach($permissions as $p)
{
	if (Settings::checkPermission($p->component, $p->name, $subject))
	{
		$userPermissions[] = $p;
	}
}

$permissionsList = formatItems($userPermissions, "{component}:{name}", ", ");

$sectionContent = GroupedQuery::create('SectionContent', "WHERE section_id IN (SELECT section_id FROM section WHERE site_id=:s) ORDER BY identifier", "section_id")
					   ->bind(":s", $site_id)
				 	   ->execute();

$sections = Query::create('Section', "WHERE site_id=:s")
				 ->bind(":s", $site_id)
				 ->execute();

$helper = new PermissionsCheckHelper($subject);

$table = new GroupedDataListView($sectionContent, "content_by_section");
$table->column("Identifier", "<strong>{identifier}</strong>", true, "width: 20%")
	  ->column("Roles", "{role:/,/, /}", true, "width: 30%")
	  ->column("Permissions", "{permissions:/,/, /}", true, "width:30%")
	  ->column("User Can Access?", array($helper, formatAccess), true, "text-align: center");

$table->groupBy($sections, "{section}", "section_id");

$table->mode = "tree";
$table->groupAsWorksheets = false;
$table->filter = true;
$table->sortable = true;
$table->excelFile = codify($config['sitename']."_permissions_{$subject->username}").".xls";
$table->onStartRow = array($helper, onStartRow);

$script .= $table->writeScript();

$tabs->writeHTML();
?>
<div class='tab_border'>
<?
$users = Query::create($userClass, "ORDER BY {$usernameField}")->execute();

echo "<p><label for='{$userPK}'>User:</label> <select name='{$userPK}' onchange=\"go('?{$userPK}=' + this.value);\">\n";

foreach($users as $u)
{
	option($u->getPrimaryKeyValue(), 
		   $u->get($usernameField)." (".$userMgr->getUserFullName($u).")", 
		   $subject->getPrimaryKeyValue());	
}

echo "</select></p>\n";
echo "<p><label>Roles:</label> {$subject->role}</p>\n";
echo "<p><label>Permissions:</label> {$permissionsList}</p>\n";

$table->drawView();
?>
</div>

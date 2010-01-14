<?php

require_once "../../include/config.inc";

require_once "../datamodel/site_user.inc";
require_once "../../include/permissions.inc";

$menu_item = "Users";
$title = "Manage Users";

$siteUsers = query(SiteUser, "ORDER BY role, username");

require_once admin_begin_page;
?>
<table class="list" width="75%">
 <tr>
  <th>Username</th>
  <th>Full Name</th>
  <th>Role</th>
 </tr>
<?
foreach($siteUsers as $siteUser)
{
?>
 <tr>
  <td><b><a href="user_form.php?user_id=<?echo $siteUser->user_id ?>">
  <?echo $siteUser->username ?></a></b></td>
  <td><?echo $siteUser->first_name ?>
  <?echo $siteUser->last_name ?></td>
  <td><?echo $siteUserRoles[$siteUser->role] ?></td>
 </tr>
<?
}
?>
</table>
<br/>
<br/>
<a href="user_form.php" class="button"> Add a New User </a>
<br/>
<br/>
<?
require_once admin_end_page;
?>
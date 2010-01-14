<?php
require_once "../../include/config.inc";

require_once "../datamodel/group.inc";
require_once "../../include/permissions.inc";

$menu_item = "Groups";
$title = "Manage Groups";


$groups = query(Group, "ORDER BY group_id");

require_once admin_begin_page;

if (count($groups) > 0) {
?>
<table class="list" width="100%">
 <tr>
  <th>Title</th>
  
 </tr>

<?
foreach($groups as $group)
{
?>
 <tr>
  <td><b><a href="grp_form.php?group_id=<?echo $group->group_id ?>">
  <?echo $group->title ?></a></b></td>
  
 </tr>
<?
}
?>
</table>
<br/>
<?
}
else { 
	?>
	<p><i>There are no groups created</i></p>
	<?
}
?>

<br/>
<a href="grp_form.php" class="button">Add a New Group</a>
<br/>
<br/>
<?
require_once admin_end_page;
?>
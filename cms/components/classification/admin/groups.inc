<?php

Fakoli::using("classification");

Fakoli::assertRole("admin");

$menu_item = "Groups";
$title = "Manage Groups";


$groups = query(Group, "ORDER BY group_id");



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
  <td><b><a href="group_form?group_id=<?echo $group->group_id ?>">
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
<a href="group_form" class="button">Add a New Group</a>
<br/>
<br/>
<?

?>
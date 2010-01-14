<?php
require_once "../../include/config.inc";

require_once "../datamodel/site.inc";
require_once "../../include/permissions.inc";

$menu_item = "Sites";
$title = "Manage Sites";


$sites = query(Site, "ORDER BY site_id");

require_once admin_begin_page;

if (count($sites) > 0) {
?>
<table class="list" width="100%">
 <tr>
  <th>Title</th>
  <th>Domain</th>
 </tr>

<?
foreach($sites as $site)
{
	$domain = canonicalizeURL($site->domain);
?>
 <tr>
  <td><b><a href="site_form.php?site_id=<?echo $site->site_id ?>">
  <?echo $site->site_name ?></a></b></td>
  <td><a target="_blank" href="<?echo $domain?>"><?echo $domain?></a></td>
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
	<p><i>There are no sites currently defined</i></p>
	<?
}
?>

<br/>
<a href="grp_form.php" class="button">Add a New Site</a>
<br/>
<br/>
<?
require_once admin_end_page;
?>
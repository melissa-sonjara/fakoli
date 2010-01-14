<?php

require_once "../../include/config.inc";

require_once "../datamodel/module.inc";
require_once "../../include/permissions.inc";

$menu_item = "Modules";

$title = "Manage Modules";

$modules = query(Module);

$count = 0;

require_once admin_begin_page;

if (count($modules) > 0)
{
?>
	
	<table class="list" width="100%">
 <tr>
  <th>Title</th>
  <th>Module Type</th>
  
   
  	 </tr>
 	
	<?
	
	foreach($modules as $module)
	{
		?>
		
		<tr><td>
	  	<a href="<?echo $module->getAdminForm()?>?module_id=<?echo $module->module_id ?>"><?echo $module->title ?></a>
	    </td>
		<td><?echo prettify($module->content_type) ?></td>
		</tr>	
		
<?			
    }
    ?>

</table>
<br/>

<?
}
else
{
	?>
	<p><i>No Modules have been added.</i></p>
	<?
}
?>
<input type="button" class="button" value="Add a Code Module" onclick="go('code_module_form.php');">&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" class="button" value="Add a Query Module" onclick="go('query_module_form.php');">&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" class="button" value="Add a HTML Module" onclick="go('html_module_form.php');">
<?
require_once admin_end_page;
?>
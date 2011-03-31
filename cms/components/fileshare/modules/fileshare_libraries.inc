<?php
/*
 * Module that lists available fileshare libraries.
 */

Fakoli::using("fileshare", "document");

$libraries = FileshareManager::getSharedLibraries();

$styles .= FileshareManager::getStyles();

?>	
<div id="fileshare_list">
<h3 class="fileshare_list">Fileshare Libraries</h3>
<ul>
<?php 
if(count($libraries) > 0)
{
	foreach($libraries as $library)
	{
?>
<li>
<a href="/fileshare_library_search?document_library_id=<? echo $library->document_library_id ?>"><?php echo $library->name ?></a>
</li>
<?php		
	}
}
?>
</div>
<div style="text-align: center">
<br>
<a href="/fileshare_library_form">Create Fileshare Library</a>
</div>

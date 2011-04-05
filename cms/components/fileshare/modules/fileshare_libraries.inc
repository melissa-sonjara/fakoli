<?php
/*
 * Module that lists available fileshare libraries.
 */

Fakoli::using("fileshare", "document");

$libraries = FileshareManager::getSharedLibraries();

$styles .= FileshareManager::getStyles();

?>	
<div id="fileshare_list">
<h3 class="fileshare_list"><img alt="library" src="/fakoli/images/icon_library.png"><a class="heading" href="/fileshare_libraries">Fileshare Libraries</a></h3>
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
<a href="/fileshare_library_form"><img alt="create_library" src="/fakoli/images/button_create_library.png"/></a>
</div>

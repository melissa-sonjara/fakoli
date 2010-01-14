<?php

require_once "../../include/config.inc";
require_once "../../framework/tab_bar.inc";
require_once "../datamodel/image.inc";
require_once "../../include/permissions.inc";

$tabs = array("Thumbnails"	=>	"images.php",
			  "Details"		=>	"image_list.php",
			);

$tabBar = new TabBar("tabs", $tabs);
$tabBar->useQueryString = false;

$menu_item = "Images";

$images = query(Image);

$count = 0;

require_once admin_begin_page;

$tabBar->writeHTML();

if (count($images) > 0)
{
?>
	<div style="overflow:auto; width:100%; height: 300px; border: 1px; border-style: solid">
	<table class="layout" width="100%"><tr>
 
 	
	<?

	foreach($images as $image)
	{
		?>
		
		<td style="font-size: 10pt; text-align: center">
		<a href="image_form.php?image_id=<?echo $image->image_id ?>"><img src="<?echo $image->thumbnail ?> " alt="Small version of image"/><br/>
		<?echo $image->title?></a></td>
	    <?php 
		++$count;
		if (($count % 3) == 0)
		{
			echo "</tr><tr>";
		}				
    }
    ?>
</tr>	
</table>
</div>
	<br/>
<?
}
else
{
	?>
	<p><i>No images have been added.</i></p>
	<?
}
?>
<input type="button" class="button" value="Add a New Image" onclick="go('image_form.php');">
<?
require_once admin_end_page;
?>
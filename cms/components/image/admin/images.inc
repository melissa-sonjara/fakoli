<?php

Fakoli::using("image");
Fakoli::usingFile("framework/tab_bar.inc");

Fakoli::assertRole("admin");

$tabs = array("Thumbnails"	=>	"images",
			  "Details"		=>	"image_list",
			);

$tabBar = new TabBar("tabs", $tabs);
$tabBar->useQueryString = false;

$menu_item = "Images";

$images = query(Image);

$count = 0;

require_once admin_begin_page;

$tabBar->writeHTML();

?>
	<div style="overflow:auto; width:100%; height: 300px; border: 1px; border-style: solid">
<?
if (count($images) > 0)
{
?>
	<table class="layout" width="100%"><tr>
 
 	
	<?

	foreach($images as $image)
	{
		?>
		
		<td style="font-size: 10pt; text-align: center">
		<a href="image_form?image_id=<?echo $image->image_id ?>"><img src="<?echo $image->thumbnail ?> " alt="Small version of image"/><br/>
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
</div>
<br/>
<input type="button" class="button" value="Add a New Image" onclick="go('image_form');">

<?
require_once admin_end_page;
?>
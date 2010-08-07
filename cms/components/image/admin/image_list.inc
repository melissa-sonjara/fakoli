<?php

Fakoli::using("image");
Fakoli::usingFile("framework/tab_bar.inc");

Fakoli::assertRole("admin");

$tabs = array("Thumbnails"	=>	"/admin/images",
			  "Details"		=>	"/admin/image_list",
			);

$tabBar = new TabBar("tabs", $tabs);
$tabBar->useQueryString = false;

$menu_item = "Images";

$images = query(Image);

$count = 0;

$title = "Images";

$tabBar->page = "image_list";

$tabBar->writeHTML();
?>
	<div style="overflow:auto; width:100%; height: 300px; border: 1px; border-style: solid">
<?
if (count($images) > 0)
{
?>
	<table class="list" width="100%">
 <tr>
  <th>Title</th>
  <th>Image File</th>
   
  	 </tr>
 	
	<?
	
	foreach($images as $image)
	{
		?>
		
		<tr><td><b><a href="image_form?image_id=<?echo $image->image_id ?>">
  <?echo $image->title ?></a></b></td>
		<td><?echo basename($image->image_file) ?></td>
		</tr>	
<?			
    }
    ?>

</table>
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

?>
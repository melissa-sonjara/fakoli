<?php
Fakoli::using("image");
Fakoli::usingFeature("auto_form");

require_once "include/image_gallery_tabs.inc";

$menu_item = "Images";

$gallery_id = checkNumeric($_GET["gallery_id"]);

if ($gallery_id)
{
	$gallery = new ImageGallery($gallery_id);
}
else
{
	$gallery = new ImageGallery();
	$gallery->owner_id = $user->user_id;
}

$gallery->filter = new ExclusionFilter("read_access", "write_access");

$tabs = imageGalleryTabs($gallery_id);

$form = new AutoForm($gallery);
$form->hide("owner_id");

if ($method == "POST")
{
	if ($form->save())
	{
		$d = $form->data->getGalleryDirectory();
		if (!is_dir($d) && !file_exists($d))
		{
			mkdir($d);
		}
		
		$tabs->queryString = "gallery_id=".$form->data->gallery_id;
		$tabs->next();
	}
}


$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?

$form->drawForm();
?>
</div>
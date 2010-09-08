<?php
Fakoli::using("image");
Fakoli::usingFeature("auto_form");

require_once "include/image_gallery_tabs.inc";

$menu_item = "Images";

$gallery_id = checkNumeric($_GET["gallery_id"]);

if (!$gallery_id) redirect("/admin/image_galleries");

$gallery = new ImageGallery($gallery_id);

$gallery->filter = new InclusionFilter("gallery_id", "read_access", "write_access");

$tabs = imageGalleryTabs($gallery_id);

$form = new AutoForm($gallery);
$form->hide("owner_id");
$readSelect = new CheckListFieldRenderer($form, "read_access", "Read Access", SiteRole::getRolesArray());
$readSelect->setSize(350, 150, 10);
$writeSelect = new CheckListFieldRenderer($form, "write_access", "Write Access", SiteRole::getRolesArray());
$writeSelect->setSize(350, 150, 10);

if ($method == "POST")
{
	if ($form->save())
	{
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
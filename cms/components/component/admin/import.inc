<?php
Fakoli::using("component");

$menu_item = "Import Configuration";
$title = "Select Site Content to be Imported";

$form = new SerializationForm("Import");

$script .= $form->writeScript();

if ($method == "POST")
{
	$xml = file_get_contents($_FILES["import_file"]["tmp_name"]);
	
	$components = $form->getSelected();
	trace("*** Importing $components", 3);
	
	$form->mgr->import($xml, $components);
	redirect("index");
}

$form->drawForm();
?>

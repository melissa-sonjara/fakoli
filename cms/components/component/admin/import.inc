<?php
Fakoli::using("component");

$menu_item = "Import Configuration";
$title = "Select Site Content to be Imported";

$form = new SerializationForm("Import");

$script .= $form->writeScript();

if ($method == "POST")
{
	$components = $form->getSelected();
	trace("*** Importing $components", 3);
	
	$tmp = $_FILES["import_file"]["tmp_name"];
	$base = basename($_FILES["import_file"]["name"]);
	$file = $config["upload_dir"].FILE_SEPARATOR.$base;
	
	move_uploaded_file($tmp, $file);
	
	$fs = filesize($file);
	trace("Import File Size: $fs bytes", 3);
	
	if ($fs > 10 * 1024 * 1024)
	{
		trace("Large Import File detected - launching background process", 3 );
		$_SESSION["xml_import_file_path"] = $file;
		$_SESSION["xml_import_components"] = $components;
		redirect("/admin/import_background");
	}
	
	$xml = file_get_contents($file);
	
	$form->mgr->import($xml, $components);
	
	unlink($file);
	redirect("index");
}

$form->drawForm();
?>

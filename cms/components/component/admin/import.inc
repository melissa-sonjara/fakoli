<?php
Fakoli::using("component");

$menu_item = "Import Configuration";
$title = "Select Site Content to be Imported";

$form = new SerializationForm("Import");

$script .= $form->writeScript();

if ($method == "POST")
{
}

$form->drawForm();
?>

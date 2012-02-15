<?php
Fakoli::using("component");

$menu_item = "Export Configuration";
$title = "Select Site Content to be Exported";

$form = new SerializationForm("Export", "/action/component/export");

$script .= $form->writeScript();

$form->drawForm();
?>

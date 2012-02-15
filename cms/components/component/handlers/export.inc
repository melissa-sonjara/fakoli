<?php
Fakoli::using("component");

Fakoli::assertRole("admin");

$file = codify($config["sitename"]."_".date("Ymd")).".xml";
	
header("Pragma: private");
header("Content-Disposition: attachment;filename=\"" . $file . "\"");
header("Content-Type: text/xml");

$mgr = new SerializationManager();

$form = new SerializationForm("Export");

$components = $form->getSelected();
trace("*** $components", 3);

echo $form->mgr->export($components);
?>
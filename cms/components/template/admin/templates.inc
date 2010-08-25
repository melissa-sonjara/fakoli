<?php
Fakoli::using("template");
Fakoli::usingFeature("directory_tree");

$mgr = new TemplateManager($config["homedir"], "templates");

$script .= $mgr->writeScript();

$mgr->drawView();
?>
<?php
Fakoli::using("component");

$component = checkIdentifier($_GET["component"]);

if ($component)
{
	$c = Fakoli::getComponent($component);
	echo "<h1>Scanning $component at $c->component_path</h1>";
	ModuleDocTagsManager::scanDirectory($c->component_path);
}
else
{
	ModuleDocTagsManager::scan();
}
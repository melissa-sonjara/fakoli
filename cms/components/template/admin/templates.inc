<?php
Fakoli::usingFeature("directory_tree");

function excludeVersionControl($folder)
{
	if (basename($folder) == ".svn") return false;
	return true;
}

function fileFilter($file)
{
	return endsWith($file, ".css") || endsWith($file, ".tpl");
}

$tree = new DirectoryTreeControl("files");
$tree->width = 500;
$tree->height = 400;
$tree->onSelectionChanged = "selectionChanged";
$tree->onDoubleClick = "downloadFile";
$tree->permissionCallback = excludeVersionControl;
$tree->icons[".tpl"] = "/fakoli/images/tpl_icon.png";
$tree->icons[".css"] = "/fakoli/images/css_icon.png";

$tree->selectMode = "file";

$tree->buildFolderTree( "templates", $config["homedir"]);
$tree->addFiles("templates", $config["homedir"], fileFilter);

$tree->writeHTML();
?>
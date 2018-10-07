<?php
Fakoli::using("versioned_content");

$class = checkIdentifier($_GET["target"]);
$itemID = checkNumeric($_GET["item_id"]);

if (!$class || !$itemID)
{
	Fakoli::end("Missing Parameter");
}

$target = new $class;
$target->load($itemID);
$adminAccess = Fakoli::getAdminAccessRoles();

if (!checkRole($adminAccess))
{
	Fakoli::end("Permission Denied");
}

try
{
	VersionedContentManager::revert($target);
	ContentVersion::deleteDraft($target);
	
	Fakoli::notifyOnNextPage("Draft changes discarded");
	Fakoli::end("OK");
}
catch(Exception $e)
{
	Fakoli::end($e->getMessage());
}

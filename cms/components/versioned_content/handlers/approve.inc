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

try
{
	VersionedContentManager::approve($target);
	
	Fakoli::notifyOnNextPage($target->prettifyClassName()." Approved");
	Fakoli::end("OK");
}
catch(Exception $e)
{
	Fakoli::end($e->getMessage());
}

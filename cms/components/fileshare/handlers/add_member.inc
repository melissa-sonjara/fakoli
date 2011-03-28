<?php
/*
 * Add a program or poc to a group email.
 * 
 * Called through the MailGroup javascript class through 
 * a request method.
 * 
 */

Fakoli::using("fileshare", "user");

$document_library_id = checkNumeric($_GET["document_library_id"]);
$user_id = checkNumeric($_GET["user_id"]);

if(!$document_library_id)
	ajaxReturn("Add Member: no document_library_id");
	
if(!$user_id)
	ajaxReturn("Add Member: no user_id");

$count = Query::create(FileshareUserXref, "WHERE user_id=:u and document_library_id=:l")
		->bind(":u", $user_id, ":l", $document_library_id)
		->executeValue("COUNT(1)");
	
if($count == 0)
{
	$xref = new FileshareUserXref();
	$xref->user_id = $user_id;
	$xref->document_library_id = $document_library_id;
	$xref->save();
	$mgr = new UserManager();
	$class_name = $mgr->getUserClass();
	$member = new $class_name($user_id);
}
else
{
	echo "";
	return;
}

if($count == 0)
{
	echo FileshareManager::constructListMember($member, $document_library_id);		
	return;
}
else
{
	echo "";
	return;
}	
?>
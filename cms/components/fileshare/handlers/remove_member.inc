<?php
/*
 * Delete a poc or a program from a mailing list.
 * 
 */

Fakoli::using("program", "mail_list");

$document_library_id = checkNumeric($_GET["document_library_id"]);
$user_id = checkNumeric($_GET["user_id"]);

trace("Remove_member:: document_library_id $document_library_id", 3);

if(!$document_library_id)
	ajaxReturn("invalid parameters");

$library = new DocumentLibrary($document_library_id);

if($user_id)
{
	trace("Remove_member:: user_id $user_id from library $document_library_id", 3);
	
	$xref = $library->getFileshareUser($user_id);
	if($xref)
		$xref->delete();
	
}
else
{
	echo "";
	return;
}	

echo "OK";
return;
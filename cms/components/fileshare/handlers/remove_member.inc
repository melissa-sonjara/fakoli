<?php
/*
 * Delete a poc or a program from a mailing list.
 * 
 */

Fakoli::using("program", "mail_list");

$group_email_id = checkNumeric($_GET["group_email_id"]);
$program_id = checkNumeric($_GET["program_id"]);
$poc_id = checkNumeric($_GET["poc_id"]);
$user_id = checkNumeric($_GET["user_id"]);

trace("Remove_member:: group_email_id $group_email_id", 3);

if(!$group_email_id)
	ajaxReturn("no group_email_id");

$groupEmail = new GroupEmail($group_email_id);

if($program_id)
{
	$xref = $groupEmail->getProgramXref($program_id);	
	if($xref)
		$xref->delete();
	
}
elseif($poc_id)
{
	trace("Remove_member:: group_email_id $group_email_id and poc_id $poc_id", 3);
	$xref = $groupEmail->getPOCXref($poc_id);
	if($xref)
		$xref->delete();
}
elseif($user_id)
{
	trace("Remove_member:: group_email_id $group_email_id and user_id $user_id", 3);
	
	$xref = $groupEmail->getUserXref($user_id);
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
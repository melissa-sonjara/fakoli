<?php
/*
 * Description: Finds users that match the search
 * criteria.

 * @author: Janice Gallant at Sonjara, Inc.
 * 
 * Date: 3/25/2011
 */

Fakoli::using("fileshare", "user");

$document_library_id = checkNumeric($_GET["document_library_id"]);
$field = mysql_escape_string($_GET["field"]);

if(!$document_library_id)
	ajaxReturn("User Search Handler: no document library id");

/*
 * Given any characters entered into the input box, search for a
 * match on first name, last name, email
 */
$search = $_GET["{$field}"]."%";

$library = new DocumentLibrary($document_library_id);

$mgr = new UserManager();
$class_name = $mgr->getUserClass();

$libraryMember = new FileshareUserXref();
$members = Query::create($class_name, "WHERE (first_name like :s or last_name like :s or email like :s) 
	and (user_id not in (select user_id from {$libraryMember->table} where document_library_id=:l)) LIMIT 100")
	->bind(":s", $search, ":l", $document_library_id)
	->execute();

if(count($members) > 0)
{	
	foreach($members as $member)
	{			
?>
<div style=""><a href="/action/fileshare/add_member?document_library_id=<?php echo $document_library_id ?>&user_id=<?php echo $member->user_id ?>"
onclick="new LibraryManager().addMemberFromProgressiveSearch(<?php echo $document_library_id ?>, <?php echo $member->user_id ?>); return false;"><b><?echo "{$member->first_name} {$member->last_name}" ?></b><br/>
<span style='font-size: 8pt'><?echo "{$member->email}" ?></span></a>
</div>
<?php 				
	}
}
?>
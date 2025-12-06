<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/*
 * Description: Finds users that match the search
 * criteria.

 * @author: Janice Gallant at Sonjara, Inc.
 * 
 * Date: 3/25/2011
 */

Fakoli::using("fileshare", "user");

$document_library_id = checkNumeric($_GET["document_library_id"]);
$field = ConnectionManager::escape($_GET["field"]);

if(!$document_library_id)
	Fakoli::end("User Search Handler: no document library id");

/*
 * Given any characters entered into the input box, search for a
 * match on first name, last name, email
 */
$search = $_GET["{$field}"]."%";

$library = new DocumentLibrary($document_library_id);

$mgr = new UserManager();
$class_name = $mgr->getUserClass();

$libraryMember = new FileshareUserXref();

$clause = "";
if(is_callable(array($mgr, 'getActiveClause')))
{
	$clause = $mgr->getActiveClause();
	if($clause) $clause .= " AND";
}

$members = Query::create($class_name, "WHERE $clause (first_name like :s or last_name like :s or email like :s) 
	and user_id not in (select user_id from {$libraryMember->table} where document_library_id=:l) LIMIT 100")
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
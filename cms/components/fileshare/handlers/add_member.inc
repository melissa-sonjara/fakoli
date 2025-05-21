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
	Fakoli::end("Add Member: no document_library_id");
	
if(!$user_id)
	Fakoli::end("Add Member: no user_id");

$count = Query::create('FileshareUserXref', "WHERE user_id=:u and document_library_id=:l")
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

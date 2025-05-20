<?php
/**\addtogroup fileshare
 * @{
 */


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

define("fileshare_only_group_members_read", 0);
define("fileshare_all_site_members_read", 1);
define("fileshare_all_visitors_read", 2);

class FileshareUserXref extends DataItem
{
	var $fields = array(
		"fileshare_user_xref_id"	=>	Number,
		"document_library_id"		=>	Number,
		"user_id"					=>	Number
	);
	
	var $relations = array(
		"DocumentLibrary"		=>	DocumentLibrary,
		"User"					=>	""
	);
	
	
	function DocumentLibrary()
	{
		return $this->getRelated(DocumentLibrary);
	}
	
	function User()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
	
	
	function __construct()
	{
		$this->primary_key = "fileshare_user_xref_id";
		$this->table = "fileshare_user_xref";

		parent::__construct(func_get_args());
	}
}
/**@}*/?>

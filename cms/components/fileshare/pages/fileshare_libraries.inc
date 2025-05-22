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

Fakoli::using("document", "fileshare");
Fakoli::usingFile("framework/data_view.inc");

$libraries = FileshareManager::getSharedLibraries();

$table = new DataListView($libraries, "libraries");
$table->column("Library", "<a href='fileshare_library_files?document_library_id={document_library_id}'>{name}</a> {description}", true)
	->column("Last Modified By", array('FileshareManager', 'getLibraryLastModifiedBy'), true)
	->column("Last Modified", array('FileshareManager', 'getLibraryLastModified'), true)
	->column("Created By", "{Owner.first_name} {Owner.last_name}", true)
	;
		  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "You do not have any file sharing libraries.";
$script .= $table->writeScript();

$table->drawView();

if ($user && !preg_match("/\\bvisitor\\b/", $user->role))
{
?>
<br/>
<div id="submit_block">
<a href="fileshare_library_form"><img alt="create_library" src="/fakoli/images/button_create_library.png"/></a>
</div>
<?
}
?>

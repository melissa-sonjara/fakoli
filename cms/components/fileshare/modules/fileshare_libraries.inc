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
 * Module that lists available fileshare libraries.
 */

Fakoli::using("fileshare", "document");

$libraries = FileshareManager::getSharedLibraries();

?>	
<div id="fileshare_list">
<h3 class="fileshare_list"><img alt="library" src="/fakoli/images/icon_library.png"><a class="heading" href="fileshare_libraries">Fileshare Libraries</a></h3>
<ul>
<?php 
if(count($libraries) > 0)
{
	foreach($libraries as $library)
	{
?>
<li>
<a href="fileshare_library_files?document_library_id=<? echo $library->document_library_id ?>"><?php echo $library->name ?></a>
</li>
<?php		
	}
}


?>
</ul>
</div>
<?php 
if ($user && !checkRole("visitor"))
{
?>
<div style="text-align: center">
<br>
<a href="fileshare_library_form"><img alt="create_library" src="/fakoli/images/button_create_library.png"/></a>
</div>
<?php 
}

?>
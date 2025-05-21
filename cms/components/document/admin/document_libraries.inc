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

Fakoli::using("document");
Fakoli::usingFile("framework/data_view.inc");

$hidden = checkNumeric($_GET["hidden"]);

if (!$hidden) 
{
	$constraint = "WHERE hidden=0";
 	$checked = "";
}
else
{
	$checked = "checked='checked'";
}

$menu_item = "Documents";

$libraries = Query::create('DocumentLibrary', "$constraint ORDER BY name")->execute();

$view = new AdminDocumentLibraryListView($libraries);

$script .= $view->writeScript();
?>
<input type='checkbox' class='checkbox' value='1' name='hidden' <?echo $checked?> onchange='go("?hidden=" + (this.checked ? "1" : "0")); return false'/>&nbsp;Show Hidden Document Libraries<br/><br/>
<?
$view->drawView();
?>
<br/>
<a href="/admin/document_library_form" class="button">Create a Document Library</a>

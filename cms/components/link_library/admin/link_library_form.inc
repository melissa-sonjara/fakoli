<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated linkation
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

Fakoli::using("link_library", "role");
Fakoli::usingFeature("auto_form");

$menu_item = "Links";

$link_library_id = checkNumeric($_GET["link_library_id"]);

if ($link_library_id)
{
	$library = new LinkLibrary($link_library_id);
}
else
{
	$library = new LinkLibrary();
}

$tabs = LinkLibraryManager::getTabs($link_library_id);

$form = LinkLibraryManager::buildLinkLibraryForm($library);

if ($method == "POST")
{
	if (!$library->library_id) 
	{
		$_POST["owner_id"] = $user->get($user->getPrimaryKey()); 
		$_POST["create_date"] = now();
	}
	
	if ($form->save())
	{
		if ($form->deleted)
		{
			redirect("link_libraries");
		}
		else
		{
			$tabs->queryString = "link_library_id=".$form->data->link_library_id;
			$tabs->next();
		}
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();
?>
</div>
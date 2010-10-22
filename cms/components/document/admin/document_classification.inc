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

Fakoli::using("document", "classification");
Fakoli::usingFile("framework/auto_form.inc");

require_once "include/document_tabs.inc";

$menu_item = "Documents";

Fakoli::assertRole("admin");

$menu_item = "Documents";

$publication_id = checkNumeric($_GET["publication_id"]);

$publication = new Document();

//$filter = new ExclusionFilter();
//$filter->add("password");

if (!checkRole("admin"))
{
	$filter->add("role");
}

$publication->filter = new InclusionFilter("publication_id", "title");

$tabs = publicationTabs($publication_id);

$form = new AutoForm($publication);
$form->hide("title");

$topics = query(Topic, "ORDER BY Topic");

$topicList = new CrossReferenceSelectFieldRenderer($form, "topics", "Topics", $topics, "<b>{topic}</b> ({details})", DocumentRegionXref);
$topicList->colspan = 2;

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->next();
	}
}
	
if ($publication_id)
{
	$publication->load($publication_id);
	$title = "Edit Document: {$publication->title}";
}
else
{
	$title =  "Add New Document";
}

$script = $form->writeScript();



$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();
?>
</div><br/>
<?

?>

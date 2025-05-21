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

Fakoli::using("classification");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

$menu_item = "Topics";
$topic_id = checkNumeric($_GET["topic_id"]);

$topic = new Topic();

$form = new AutoForm($topic);
$form->required("topic");
$form->allowDelete = true;
$redirect = "/admin/topics";
$form->button("Cancel", $redirect);

$groups = query('Group', "ORDER BY group_id"); 

$grpList = new CrossReferenceSelectFieldRenderer($form, "groups", "Associate Groups", $groups, "{title}", GroupTopicXref);
$grpList->colspan = 2;
$grpList->height = 120;

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/topics");	
	}
}

if ($topic_id)
{
	$topic->load($topic_id);
	$title = "Edit Topic Details for {$topic->topic}";
}
else
{
	$title = "Add a New Topic";
}

$script .= $form->writeScript();



$form->drawForm();


?>

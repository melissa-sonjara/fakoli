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

Fakoli::using("forum");
Fakoli::usingFile("framework/auto_form.inc");

$forum_id = checkNumeric($_GET["forum_id"]);

if ($forum_id)
{
	$forum = new Forum($forum_id);
	$page->page_title = "Edit Forum: $forum->title";
}
else
{
	$forum = new Forum();
	$page->page_title = "Add a New Discussion Forum";
}


$form = new AutoForm($forum);
$form->required("identifier", "title", "teaser", "description");
$form->hide("created_date", "owner_id");

if ($method == "POST")
{
	if (!$forum_id)
	{
		trace("***** CREATING NEW FORUM ******: ".now(), 3);

		$_POST['owner_id'] = $user->get($user->getPrimaryKey());
		$_POST['created_date'] = now();
	}	

	if ($form->save())
	{
		if (!$forum_id)
		{
			$jn = new ForumSiteXref();
			$jn->forum_id = $form->data->forum_id;
			$jn->site_id = $site->site_id;
			$jn->save();
		}
		
		redirect("forum_list");
	}
}

$script .= $form->writeScript();

$form->drawForm();
?>
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

$forum_id = checkNumeric($_GET["forum_id"]);

$forum = new Forum($forum_id);

$page->page_title = $forum->title;

$view = new ForumDetailsView($forum, "forum_topic");
$view->table->cssStyle = "width: 700px; clear: both";

$script .= $view->writeScript();
?>
<div><?echo $forum->description?></div>
<?
$view->drawView();

?>
<br/>
<div style="width: 100%"><div style="float: right">
<?php 
if($user)
{
?>
<a class="button" href="/forum_message_form?forum_id=<?echo $forum_id?>">Create a Topic</a>
<?
}
if (checkRole("admin,moderator"))
{
?>
&nbsp;&nbsp;<a class="button" href="/forum_form?forum_id=<?echo $forum_id?>">Edit this Discussion Forum</a>
<?
	if($forum->allowDelete())
	{
?>
&nbsp;&nbsp;<a class="button" href="/action/forum/forum_delete?forum_id=<?echo $forum_id?>" onclick="return confirm('Are you sure you want to delete this forum?');">Delete this Discussion Forum</a>
<?php		
	}		
}
?>
</div>
<a class="button" href="/forum_list">&laquo; Back to Forum List</a>
</div>
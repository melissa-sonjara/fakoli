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

Fakoli::using("user", "forum", "attachment");

$forum_id = checkNumeric($_GET["forum_id"]);
$topic_id = checkNumeric($_GET["topic_id"]);

if (!$topic_id) redirect("/");

$topic = new ForumTopic($topic_id);
$message = $topic->FirstPost();
$page->page_title = $message->title;

$view = new ForumTopicView($topic);

echo $view->writeScript();

$view->drawView();
?>
	<button style="float: left" class="button" onclick="go('/forum_details?forum_id=<?echo $topic->forum_id?>');"> &laquo; Back to Topic List</button>	
	<button style="float: left; margin-left: 425px" class="button" onclick="go('forum_message_form?forum_id=<?echo $topic->forum_id?>&topic_id=<? echo $topic->topic_id ?>&parent_id=<?echo $topic->message_id?>')"> Reply to this topic </button>
<?

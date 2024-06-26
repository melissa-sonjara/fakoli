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

$constraint = checkRole("admin") || checkRole("moderator") ? "ORDER BY sort_order, title" : "WHERE published=1 ORDER BY sort_order, title";

$forumSummaries = $site->ForumSummaries($constraint);

$forumListView = new ForumListView($forumSummaries, "forum_details", "forum_topic");
$forumListView->table->cssStyle="width: 100%; clear: both";

$script .= $forumListView->writeScript();

$forumListView->drawView();

if (checkRole("admin,moderator"))
{
?>
<br/>
<button onclick="go('forum_form')" class="button">Create New Forum</button>
<? 
}
/*
elseif(checkRole('member,registeredusers'))
{
?>
<br/>
<button onclick="go('forum_request_form')" class="button">Request New Forum</button>
<?
}
*/
?>
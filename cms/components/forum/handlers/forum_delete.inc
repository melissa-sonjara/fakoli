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
Fakoli::assertRole("admin,moderator");

$forum_id   = checkNumeric($_GET["forum_id"]);

if (!$forum_id)
	redirect($_SERVER['HTTP_REFERER']);

$forum = new Forum($forum_id);

/*
 * We already know there are no messages b/c we were allowed to 
 * delete, but we need to delete all related records:
 * 
 *  ForumSiteXref
 *  ForumTopic
 *  ForumSubscription
 */
if($forum->allowDelete())
{
	$xrefs = query(ForumSiteXref, "WHERE forum_id={$forum_id}");
	if(count($xrefs) > 0)
	{
		foreach($xrefs as $xref)
			$xref->delete();
	}
	
	$topics = query(ForumTopic, "WHERE forum_id={$forum_id}");
	trace(" ForumID: $forum_id");
	if(count($topics) > 0)
	{
		foreach($topics as $topic)
		{
			$topic->delete();
		}
	}
	
	$subscriptions = query(ForumSubscription, "WHERE forum_id={$forum_id}");
	if(count($subscriptions) > 0)
	{
		foreach($subscriptions as $subscription)
		{
			$subscription->delete();
		}
	}
	
	$forum->delete();
}

redirect($_SERVER['HTTP_REFERER']);

?>
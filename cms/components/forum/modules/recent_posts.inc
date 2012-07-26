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
 * Shows the most recent posts from each discussion forum.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 6/16/2011
 */

Fakoli::using("forum");
		
$topics = Query::create(ForumTopic, "WHERE message_id not in (select message_id from forum_message where deleted=true) ORDER BY date_created LIMIT 5")
		->execute();
			
$messages = indexedQuery(ForumMessage, "WHERE deleted=false AND parent_id=0 ORDER BY date_posted", "topic_id");		

echo "<div class=\"module_header\">Recent Forum Posts</div>\n";
echo "<div class=\"module_list\">\n";	
if(count($messages) > 0)
{
	echo "<ul>\n";
	foreach($topics as $topic)
 	{  	
 		if (!array_key_exists($topic->topic_id, $messages))
 			continue;
	
		$message = $messages[$topic->topic_id];
		echo "<li><a href='forum_topic?forum_id={$topic->forum_id}&topic_id={$topic->topic_id}'>{$message->title}</a></li>\n";
 	}		
	echo "</ul>\n";
	echo "</div>\n";
}
else
	echo "<p>There are no discussion forum posts.</p>\n"; 
		
?>
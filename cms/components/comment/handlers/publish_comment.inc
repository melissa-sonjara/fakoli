<?php
/*
 * publish/Un-Publish a comment.
 * 
 * 
 */

Fakoli::using("comment");

$comment_id = checkNumeric($_GET["comment_id"]);

$comment = new Comment();

if ($comment_id)
{
	$comment->load($comment_id);
}

if(!$comment->published)
{
	$comment->published = true;
}
else
{
	$comment->published = false;
}

if ($comment->save())
{
	Fakoli::end("OK");
}
else
{
	Fakoli::end("save failed");
}

?>

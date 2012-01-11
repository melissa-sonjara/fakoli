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

Fakoli::using("comment");
Fakoli::usingFeature("auto_form", "paged_list");

/*
 * View for displaying list of comments linked to an obj and
 * a form for entering a new comment;
 * 
 * @obj - object that is linked to comments - must have relation "Comments"
 * (e.g., participant)
 * 
 * 
 * @xrefClass - the class that links the obj to the Comment table, e.g.,
 * ParticipantCommentXref
 * 
 * @title - the title to display above the comment section.
 */

class CommentView
{
	var $obj;
	var $list;
	var $form;
	var $xrefClass;
	var $title;
	
	function CommentView($obj, $xrefClass, $title = "")
	{
		$this->obj = $obj;
		$this->xrefClass = $xrefClass;
		$this->title = ($title) ? $title : "Comments";
		$this->list = $this->buildCommentList($obj);
		$this->form = $this->buildCommentForm();
	}
	
	function buildCommentList($obj)
	{
		$comments = $obj->Comments("ORDER BY date_posted DESC");

		$list = new PagedList($comments, "comment_list", array($this, formatComment));
		$list->CSSclass = "Standard";
		$list->styles = "clear: both";
		$list->pageSize = 5;

		return $list;
	}
	
		
	function formatComment($comment)
	{
		$author = ($comment->user_id) ? $comment->format("{User.first_name} {User.last_name}") : $comment->author;
		
		if($comment->title)
			$out .= "{$comment->title}<br>";

		if($comment->isEditable())
			$edit = " <a href='#' onclick=\"new Comment().showCommentDialog({comment_id}, '{$this->xrefClass}'); return false\">(edit)</a>";
		$out .= $comment->format("<b>{$author}</b> on {date_posted:m/d/Y} wrote: {description}$edit");
		
		return $out;
	}
	
	function writeScript()
	{
		$script .= $this->list->writeScript();
		$script .= $this->form->writeScript();
		
		return $script;
	}
	
	function drawView()
	{
		echo "<h3>{$this->title}</h3>\n";
		if(count($this->list->items) > 0)
			$this->list->drawList();
	}
	
	function drawForm()
	{
		$this->form->drawForm();
	}
	
	function buildCommentForm()
	{
		global $user;
		
		$comment = new Comment();
		$comment->user_id = $user->user_id;
		$form = new AutoForm($comment);
		$form->required("description");
		$form->alias("description", "comment");
		$form->hide(user_id);
		
		if ($user) 
		{
			$form->hide("author");
		}
		else
		{
			$form->required("author");
		}
		
		return $form;
	}
	

	function save() 
	{
		$form = $this->form;
		
		if($form->save())
		{
			$xref = new $this->xrefClass;
		
			$obj = $this->obj;
			$pk =  $this->getMatchingPK($xref, $obj);
			
			$xref->set($pk, $obj->get($pk));
			$xref->set("comment_id", $form->data->get("comment_id"));
			$xref->save();	
			
			return true;	
		}
	}	
	
	function getMatchingPK($xref, $obj)
	{
		$pks = $obj->getPrimaryKeyList();
		foreach($pks as $pk)
		{
			if ($xref->hasField($pk)) return $pk;
		}
		
		throw new DataItemException("No matching foreign key in xref table");
	}
	
}

?>
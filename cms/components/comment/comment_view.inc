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

Fakoli::using("comment", "email");
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
	var $component;
	var $title;

	/**
	 *
	 * @param obj $obj any DataItem class that can link to Comment
	 * @param String $xrefClass DataItem xref class that links obj to Comment
	 * @param String $component
	 * @param String $title
	 */
	function CommentView($obj, $xrefClass, $component, $title = "")
	{
		$this->obj = $obj;
		$this->xrefClass = $xrefClass;
		$this->component = $component;
		$this->title = ($title) ? $title : "Comments";
		$this->list = $this->buildCommentList($obj);
		$this->form = $this->buildCommentForm();
	}

	static function fromGET()
	{
		$adapter = CommentManager::getAdapter();
		
		if ($adapter)
		{
			$parent = $adapter->createParent();
			return new CommentView($parent, $adapter->xrefClass, $adapter->component);
		}
	}
	
	function buildCommentList($obj)
	{
		if(checkRole("admin"))
		{
			$comments = $obj->Comments("ORDER BY date_posted DESC");	
		}
		else
		{
			$comments = $obj->Comments("Where published=1 ORDER BY date_posted DESC");
		}
		$list = new PagedList($comments, "comment_list", array($this, formatComment));
		$list->CSSclass = "Standard";
		$list->styles = "clear: both";
		$list->pageSize = 5;

		return $list;
	}


	function formatComment($comment)
	{
		$author = ($comment->user_id) ? $comment->format("{User.first_name} {User.last_name}") : $comment->author;

		if($comment->published)
		{
			$out = "<div id='comment{$comment->comment_id}' class='message'>\n";
		}
		else
		{
			$out = "<a name='comment{$comment->comment_id}'></a><div id='comment{$comment->comment_id}' class='message_unpublished'>\n";
		}
		$out .=" <div class='title'>";

		if ($comment->title)
		{
			$out .= "<strong>{$comment->title}</strong><br/>";
		}

		$out .= "<span class='small'>Posted by $author at {$comment->date_posted}</span>\n </div>\n <div class='message_content'>";
		
		if($comment->isEditable() || (checkRole("admin") && !Settings::getValue("comment","publish_automatically")))
		{
			$edit .= $this->drawButtons($comment);
		}
		
		$out .= $comment->format("{description:stripHTML}$edit");
		$out .= "</div></div></a>";
		return $out;
	}

	function drawButtons($comment)
	{
		$edit = "<div class='button_row'>\n";

		if($comment->isEditable())
		{
			$edit .= "<a href='#' class='button' onclick=\"new Comment().showCommentDialog({comment_id}, '{$this->xrefClass}', '{$this->component}'); return false\"> Edit </a>&nbsp;&nbsp;";
		}
		if(checkRole("admin") && !Settings::getValue("comment","publish_automatically"))
		{
			if($comment->published)
			{
				$edit .= "<a href='#' class='button' onclick=\"new Comment().commentPublish({comment_id}); return false;\"> Un-Publish </a>\n";
			}
			else
			{
				$edit .= "<a href='#' class='button' onclick=\"new Comment().commentPublish({comment_id}); return false;\"> Publish </a>\n";
			}
		}
		$edit .= "<div style='clear:both'></div></div>\n";
		
		return $edit;
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

		$key_name = $this->obj->getPrimaryKey();
		$key_value = $this->obj->get($key_name);

		$comment = new Comment();
		if(Settings::getValue("comment", "publish_automatically"))
		{
			$comment->published = true;
		}
		$form = new AutoForm($comment, "POST", "/action/comment/comment_form?comment_id=&xref_class={$this->xrefClass}&key_name=$key_name&key_value=$key_value&xref_component={$this->component}", "Comment_form");
		$form->ajaxSubmit("function(result) {new Comment().commentFormResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");

		$form->required("description");
		$form->alias("title", "Subject", "author", "Your Name");
		$form->alias("description", "Comment");
		$form->hide(user_id, published);
		$form->onSaveComplete = array($this, addXrefRecord);
		
		if ($user)
		{
			$comment->user_id = $user->get($user->primary_key);
			$form->hide("author");
		}
		else
		{
			$comment->user_id = 0;
			$form->required("author");
		}

		return $form;
	}

	function addXrefRecord($form)
	{
		$xref = new $this->xrefClass;
		$xref->set($this->obj->primary_key, $this->obj->get($this->obj->primary_key));
		$xref->set($form->data->primary_key, $form->data->get($form->data->primary_key));
		$xref->save();
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
	
	/**
	 * Called by comment_form handler to save xref and send email
	 * for new comments.
	 * 
	 * AutoForm's obj can by any obj that links to Comment (e.g,
	 * Article)
	 * 
	 * @param obj $form AutoForm
	 */
	static function onSaveNewComment(&$form)
	{
		$xref_class = $form->data->get("xref_class");
		$xref = new $xref_class;
		
		$obj = $form->data;
		
		// The primary key name of the parent obj (e.g., Article)
		$key_name = $obj->get("key_name");
		$key_value =  $obj->get("key_value");
		
		$xref->set($key_name, $key_value);
		$xref->set("comment_id", $obj->get("comment_id"));
		
		if (method_exists($xref, verifyEnabled))
		{
			if (!$xref->verifyEnabled($key_value))
			{
				Fakoli::end("No SPAM for you today, Mister!");
			}
		}
		
		// Wait until after verified, if applicable, to save
		$xref->save();

		$confirmation_message = "Thank you for your comment.";
		if(!Settings::getValue("comment", "publish_automatically"))
		{
			sendEmailUsingEmailManager($xref, 'new_comment');
			$confimation_message = "Thank you. Your comment has been submitted to the moderator.";
		}

		$form->data->set("confirmation_message", $confirmation_message);
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
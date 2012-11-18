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

Fakoli::usingFile("framework/field_renderers.inc");

class CommentFieldRenderer extends FieldRenderer
{
	var $comment;
	var $list;
	var $xrefClass;
	var $component;
	
	function CommentFieldRenderer(&$parent, $field, $xrefClass, $component, $comments = null, $readOnly = false)
	{
		$this->FieldRenderer($parent);
		$this->hideLabel = true;
		$this->parent->add($this, $field);
		$this->xrefClass = $xrefClass;
		$this->component = $component;
		$this->field = $field;
		
		$this->list = $this->buildCommentList($comments);
		$this->comment = $this->createComment();	
		$this->addField($this->field);
		
		if(!$readOnly)
		{
			$this->addTitleField();
			$this->addAuthorField();
			$this->addCommentField();	
		}
	}
	
	function createComment()
	{
		global $user;
		
		$comment = new Comment();
		if(!checkRole("admin"))
		{
			$comment->published = true;
		}

		if($user)
		{
			$comment->user_id = $user->user_id;
		}
		
		return $comment;
	}
	
	function addTitleField()
	{
		$field = $this->getTitleField();
		$this->addField($field);
		$this->parent->add(new StringFieldRenderer($this->parent), $field);
	}
	
	/**
	 * Override title field with a subject drop down, if desired
	 * 
	 * @param $options
	 * @param $label
	 */
	function subjectSelectField($options, $label = "Subject")
	{
		$field = $this->getTitleField();
		$this->parent->override($field, $label = "", new SelectFieldRenderer($this->parent, $field, $label, $options));
	}
	
	function addField($field)
	{
		$filter = $this->parent->data->filter;
		if ($filter && $filter->isExcluded($field))
		{
			if($filter->type == "InclusionFilter")
			{ 
				$filter->add($field);
			}
			else
			{
				$filter->remove($field);
			}
		}		
	}
	
	function addCommentField()
	{
		$field = $this->getCommentField();
		$this->parent->add(new TextFieldRenderer($this->parent), $field);
		$this->parent->alias($field, "Comment");
		$this->addField($field);
	}
	
	function addAuthorField()
	{
		$field = $this->getAuthorField();
		global $user;
		
		if(!$user)
		{
			$this->parent->add(new StringFieldRenderer($this->parent, $field));
			$this->addField($field);
		}
	}
	
	function addGroup($title = "")
	{
		if(!$title) $title = "Comments";
		$this->parent->group("Comments", $this->field, $this->getTitleField(), $this->getCommentField());
	}
	
	function getCommentDialog()
	{
		return "comment_dialog";	
	}
	
	function getTitleField()
	{
		return "comment_title";
	}
	
	function getCommentField()
	{
		return "comment_description";
	}
	
	function getAuthorField()
	{
		return "comment_author";
	}
	
	function buildCommentList($comments)
	{
		if(!is_array($comments)) 
		{
			$comments = $this->getComments();
		}
		
		$field = $this->field;
		$list = new PagedList($comments, "{$this->parent->id}_{$field}", array($this, formatComment));
		$list->styles = "clear: both";
		$list->CSSclass = "comment";
		$list->paginate = false;
		
		return $list;
	}
	
	function getComments()
	{
		if(!checkRole("admin"))
		{
			$constraint = "WHERE published=true";
		}
		return $this->parent->data->Comments("$constraint ORDER BY date_posted DESC");
	}
	
	function formatComment($comment)
	{
		$field = $this->field;
		$author = ($comment->user_id) ? "{User.first_name} {User.last_name}" : "{author}";

		$class = ($comment->published) ? "message" : "message_unpublished";
		$out .= "<div id='comment_{comment_id}' class='$class'>\n";
		$out .=" <div class='title'>";

		if ($comment->title)
		{
			$out .= "<strong><span id='comment_{comment_id}_title'>{title:prettify}</span></strong><br/>\n";
		}

		$out .= "<span class='small'>Posted by $author at {date_posted:short}</span>\n </div>\n";
		$out .= "<div class='message_content'><span id='comment_{comment_id}_message'>{description:stripHTML}</span>";
		
		if($comment->isEditable() && !$this->parent->isReadOnly($field))
		{
			$out .= "&nbsp;<a href='#' onclick=\"commentMgr.showCommentDialog({comment_id}); return false\">(edit)</a>";
		}
		
		$out .= "</div></div>";
		 
		$out = $comment->format($out);

		return $out;
	}
	
	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";

		//echo $this->list->writeScript();
		
		$item = $this->parent->data;
		$itemPk = $item->getPrimaryKey(); 
		$item_id = $item->$itemPk;
		$commentDialog = $this->getCommentDialog();

?>
<script type="text/javascript" src="/components/comment/js/comment_manager.js"></script>
<script type="text/javascript">
var commentMgr;

window.addEvent('domready', function()
{
	commentMgr = new CommentManager(
			'<?php echo $itemPk ?>',
			<?php echo $item_id ?>,
			'<?php echo $this->component ?>',
			'<?php echo $commentDialog ?>'			 
	);
});

</script>
<?
	}
		
	function renderField($field)
	{
		$this->list->listCSS = $this->listCSS;
		
		if(count($this->list->items) > 0)
		{
			$this->list->drawList();
		}
		// title and comment fields rendered by parent
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

	function postProcess($field = "")
	{
		$commentField = $this->getCommentField();
		$titleField = $this->getTitleField();
		$authorField = $this->getAuthorField();
		trace("CommentFieldRenderer postProcess title field $titleField value ". $_POST[$titleField], 3);
		
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($commentField) || array_key_exists($commentField, $this->parent->hidden)) return "";
		
		$xref = new $this->xrefClass;
		
		$obj = $this->parent->data;
		$pk =  $this->getMatchingPK($xref, $obj);
	
		$commentMessage = $_POST[$commentField];
		
		if(!$commentMessage)
		{
			trace("CommentFieldRenderer no comment entered in comment field {$commentField}", 3);
			return;
		}
		$comment = $this->comment;
		$comment->title = $_POST[$titleField];
		$comment->description = $commentMessage;
		
		$author = $_POST[$authorField];
		if($author)
		{
			$comment->author = $author;
		}
		$comment->save();
		
		$xref->set($pk, $obj->get($pk));
		$xref->set("comment_id", $comment->comment_id);
		$xref->save();
	}	
	
	function renderReadOnly($comment)
	{
		$this->readOnly = true;
		$this->renderField($this->field);
	}	
}
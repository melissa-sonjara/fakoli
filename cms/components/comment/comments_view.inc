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

Fakoli::using("article");
Fakoli::usingFeature("auto_form");

class CommentsView
{

	function CommentsView($title = null)
	{
		$this->title = $title || "Comment on this article";
		
	
	}
	function CommentForm($article_id, $site_user_id)
	{
		global $user;
		
		$comment = new Comment();
		$comment->user_id = $site_user_id;
		$this->form = new AutoForm($comment);
		$this->form->getRenderer("description")->columns = 63;
		$this->form->required("title", "description");
		$this->form->alias("description", "comment");
		$this->form->hide(user_id);
		
		if ($user) 
		{
			$this->form->hide("author");
		}
		else
		{
			$this->form->required("author");
		}
		
		$this->form->writeScript();
		$this->form->drawForm();
	}

	function saveComments() {

		$this->form->save();
		$article_id = checkNumeric($_GET["article_id"]);
		$newsItemComment = new ArticleCommentXref();
		$newsItemComment->comment_id = $this->form->data->comment_id;
		$newsItemComment->article_id = $article_id;
		$newsItemComment->save();
	
	}

	function displayComments()
	{
			   
		$article_id = checkNumeric($_GET["article_id"]);
	
		$article = new Article($article_id);

		$comments = $article->Comments("WHERE article_id=$article_id");
		if(count ($comments)) {
		foreach ($comments as $comment) {
		 
		 ?>
		<div id="comment_content">
		<?php 
		     echo "<b>";
			echo $comment->author;
			echo "</b>";
			echo " on $comment->date_posted said: ";
			echo "<br/>";
		    
			echo $comment->description;
			echo "<br>";
		?></div>
		<?php	
		}
		
	
	}

}
}
?>
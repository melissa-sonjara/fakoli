<?php
require_once $config['homedir']."/cms/datamodel/comment.inc";
require_once $config['homedir']."/cms/datamodel/news_item.inc";
require_once  $config['homedir']."/framework/auto_form.inc";



class CommentsView
{

	
	function CommentsView($title = null)
	{
		$this->title = $title || "Comment on this article";
		
	
	}
	function CommentForm($news_item_id, $site_user_id)
	{
		$comment = new Comment();
		$comment->user_id = $site_user_id;
		$this->form = new AutoForm($comment);
		$this->form->getRenderer("description")->columns = 63;
		$this->form->required("title", "description", "author");
		$this->form->hide(user_id);
		$this->form->writeScript();
		$this->form->drawForm();
	}

	function saveComments() {

		$this->form->save();
		$news_item_id = checkNumeric($_GET["news_item_id"]);
		$newsItemComment = new NewsItemCommentXref();
		$newsItemComment->comment_id = $this->form->data->comment_id;
		$newsItemComment->news_item_id = $news_item_id;
		$newsItemComment->save();
	
	}

	function displayComments()
	{
			   
		$news_item_id = checkNumeric($_GET["news_item_id"]);
	
		$news_item = new NewsItem($news_item_id);

		$comments = $news_item->Comments("WHERE news_item_id=$news_item_id");
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
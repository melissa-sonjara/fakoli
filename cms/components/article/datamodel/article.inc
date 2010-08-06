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
 
/**
 * Defines the Article class.
 */
class Article extends DataItem
{
	var $fields = array("article_id"	=>	Number,
						"title"			=>	String,
						"image_id"		=>	Number,
						"article_type"	=>	String,
						"teaser"		=>	HTML,
						"message"		=>	HTML,
						"created_date"	=>	Date,
						"archive_date"  =>  Date,
						"last_modified"	=>	Timestamp,
						"published"		=>	Boolean,
						"allow_comment" =>	Boolean,
						"headline"		=>	Boolean,
						"tags"			=>	String,
						"user_id"		=>	Number);

	var $relations = array(	"Image" 	=> Image,
							"Sites"		=> Site,
							"Comments"	=> Comment,
							"Author"	=> SiteUser);
	
	function Image()
	{
		return $this->getRelated(Image);
	}
	
	function Sites($constraint = "")
	{
		return $this->crossReference(Site, ArticleSiteXref, $constraint);
	}
	
	function Comments($constraint = "")
	{
		return $this->crossReference(Comment, ArticleCommentXref, $constraint);
	}
	
	function Author()
	{
		if (!$this->user_id) return null;
		return $this->getRelated(SiteUser);
	}
	
	function Article()
	{
		$this->table = "article";
		$this->primary_key = "article_id";
		$this->pretty_class_name = "Article";
		
		$this->DataItem(func_get_args());
	}
}

class ArticleSiteXref extends DataItem
{
	var $fields = array("article_site_xref_id"	=>	Number,
						"article_id"				=>	Number,
						"site_id"					=>	Number);
	
	var $relations = array(	"Article"	=>	Article,
							"Site"		=>	Site);
	
	function Article()
	{
		return $this->getRelated(Article);
	}
	
	function Site()
	{
		return $this->getRelated(Site);
	}
	
	function ArticleSiteXref()
	{
		$this->table = "article_site_xref";
		$this->primary_key = "article_site_xref_id";
		$this->DataItem(func_get_args());
	}
}

class ArticleCommentXref extends DataItem
{
	var $fields = array("article_comment_xref_id"	=>	Number,
						"article_id"				=>	Number,
						"comment_id"				=>	Number);
	
	var $relations = array(	"Article"	=>	Article,
							"Comment"		=>	Comment);
	
	function Article()
	{
		return $this->getRelated(Article);
	}
	
	function Comment()
	{
		return $this->getRelated(Comment);
	}
	
	function ArticleCommentXref()
	{
		$this->table = "article_comment_xref";
		$this->primary_key = "article_comment_xref_id";
		$this->DataItem(func_get_args());
	}
}

?>
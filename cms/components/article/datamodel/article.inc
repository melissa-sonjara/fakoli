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
 
Fakoli::using("blog", "user");

/**
 * Defines the Article class.
 */
class Article extends DataItem implements Searchable
{
	var $fields = array("article_id"	=>	Number,
						"title"			=>	String,
						"image_id"		=>	Number,
						"article_type"	=>	String,
						"teaser"		=>	HTML,
						"message"		=>	HTML,
						"created_date"	=>	Date,
						"archive_date"  =>  Date,
						"publish_date"  =>  Date,
						"last_modified"	=>	Timestamp,
						"published"		=>	Boolean,
						"allow_comment" =>	Boolean,
						"headline"		=>	Boolean,
						"tags"			=>	String,
						"author_id"		=>	Number,
						"author"		=>	String,
						"sort_order"	=>	Number
			);

	var $relations = array(	"Image" 	=> ImageRecord,
							"Sites"		=> Site,
							"Comments"	=> Comment,
							"Author"	=> "");
	
	function Image()
	{
		return $this->getRelated(ImageRecord);
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
		$mgr = new UserManager();
		return $mgr->getUser($this->author_id);
	}
	
	function getAuthorName()
	{
		return $this->Author()->getFullName();
	}
	
	function getBlog()
	{
		$blog = Query::create(Blog, "WHERE title=:type")
					 ->bind(":type", $this->article_type)
					 ->executeSingle();
					 
		return $blog;
	}
	
	function getViewIdentifier()
	{
		try
		{
			$blog = $this->getBlog();
			return $blog->identifier;
		}
		catch(DataNotFoundException $e)
		{
			return "article_view";
		}
	}
	
	function Article()
	{
		$this->table = "article";
		$this->primary_key = "article_id";
		$this->pretty_class_name = "Article";
		
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 		
				
		$this->DataItem(func_get_args());
	}
	
	function search($params)
	{
		trace("Searching Articles", 3);
		
		if (is_object($params))
		{
			$search = clone $params;
			$search->target = $this;
			
			if (!$search->get("text:like"))
			{
				$search->copyField("keywords:like", "text:like");
			}

			$search->remapField("text", "message");
			
			$search->remapField("keywords", "tags");
			$constraint = $search->generateConstraint();
			$constraint .= " AND published=1";
			
			$articles = Query::create(Article, $constraint)
						  ->execute();
		}
		else
		{
			$params = "%$params%";
			$articles = Query::create(Article, "WHERE title like :a OR teaser LIKE :b OR message like :c")
						  ->bind(":a", $params, ":b", $params, ":c", $params)
						  ->execute();
		}
							  
		return SearchManager::wrap($articles, ArticleSearchResult);
	}
}

class ArticleSearchResult extends AbstractSearchResult
{
	var $item;
	
	function ArticleSearchResult($item)
	{
		$this->item = $item;
	}
	
	function relevance()
	{
		return 1;
	}
	
	function title()
	{
		return $this->item->title;
	}
	
	function date()
	{
		return $this->item->created_date;
	}
	
	function summary()
	{
		return $this->item->format("<h4>{title}</h4><p><em>{author} {created_date:short}</em></p><a href='{getViewIdentifier()}?article_id={article_id}'>Read More</a>");
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
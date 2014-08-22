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
						"message"		=>	HTML,
						"article_type"	=>	String,
						"image_id"		=>	Number,
						"teaser"		=>	HTML,
						"created_date"	=>	Date,
						"post_date"		=>	Date,
						"archive_date"  =>  Date,
						"publish_date"  =>  Date,
						"last_modified"	=>	Timestamp,
						"published"		=>	Boolean,
						"allow_comment" =>	Boolean,
						"headline"		=>	Boolean,
						"tags"			=>	String,
						"author_id"		=>	Number,
						"author"		=>	String,
						"publication"	=>	String,
						"sort_order"	=>	Number
			);

	var $relations = array(	"Image" 	=> ImageRecord,
							"Sites"		=> Site,
							"Comments"	=> Comment,
							"Author"	=> "",
							"Blog"		=>	Blog,
			);
	
	var $fieldAliases = array("message"	=>	"Article Content");
	
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
	
	/**
	 * We retrieve the blog related to the article by matching the
	 * article_type field to the blog's title field. This function
	 * may return null as an article may not be related to any blog.
	 * 
	 * Used for merge codes and for comment xref function verifyEnabled
	 */
	function Blog()
	{
		$blogs = Query::create(Blog, "WHERE title=:article_type")
			->bind(":article_type", $this->article_type)
			->execute();
				
		return count($blogs) ? $blogs[0] : null;
	}
	
	function Author()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->author_id);
	}
	
	function getAuthorName()
	{
		if ($this->author_id)
		{
			return $this->Author()->getFullName();
		}
		else return $this->author;
	}
	
	function getBlog()
	{
		$blogs = Query::create(Blog, "WHERE title=:type")
					 ->bind(":type", $this->article_type)
					 ->execute();
					 
		return count($blogs) ? $blogs[0] : null;
	}
	
	function getURL()
	{
		$blog = $this->getBlog();
		if($blog)
		{
			return $blog->getURL() . "?article_id=" . $this->article_id;
		}
		else
		{
			return "article?article_id=" . $this->article_id;
		}
	}
	
	function getBlogSubscribers()
	{
		$blogSubscribers = Query::create(BlogSubscriber, "where blog_id=:blog_id")
								->bind(":blog_id", $this->getBlog()->blog_id)
								->execute();
		$email_list_array = array();
		foreach($blogSubscribers as $blogSubscriber)
		{
			$email_list_array[] = $blogSubscriber->subscriber_email;
		}
		$email_list = implode(",", $email_list_array);
		
		return $email_list;
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
	
	function formatPostDate()
	{
		if ($this->post_date)
		{
			return $this->format("{post_date:daylong}");
		}
		
		if ($this->publish_date)
		{
			return $this->format("{publish_date:daylong}");
		}
		
		return $this->format("{created_date:daylong}");
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
	
	function search($params, $range = null)
	{
		trace("Searching Articles", 3);
		
		if ($range)
		{
			$range = " AND {$this->primary_key} IN (".implode($range, ", ").")";
		}
		else
		{
			$range = "";
		}
		
		if (is_object($params))
		{
			$search = clone $params;
			$search->target = $this;
			
			$search->remapField("keywords", "tags");
			
			if (!$search->get("text", "like"))
			{
				$search->secondaryFields("tags", "message");
			}
			else
			{
				$search->remapField("text", "message");
			}
			
			$search->remapField("publication_date", "publish_date");
			
			$constraint = $search->generateConstraint();
			$constraint .= $constraint ? " AND published=1" : " WHERE published=1";
			
			$constraint .= $range;
			
			$articles = Query::create(Article, $constraint)
						  ->execute();
		}
		else
		{
			$params = "%$params%";
			$articles = Query::create(Article, "WHERE (title like :a OR teaser LIKE :b OR message like :c) AND published=1 $range")
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
	
	function getPrimaryKey() { return $this->item->getPrimaryKey(); }
	function get($field) { return $this->item->get($field); }
	function prettifyClassName($plural = false) { return $this->item->prettifyClassName($plural); }
	function format($format) { return $this->item->format($format); }
	
	function relevance()
	{
		return 0.5;
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
		$details = "<p><em>{author} {created_date:short}</em></p>";
		$href = "href='{getViewIdentifier()}?article_id={article_id}'";
		
		if(Settings::getValue("search", "show_text_fragment"))
		{
			return $this->item->format("<h4>{title}</h4>{$details}{teaser}<a {$href}>Read More</a>");
		}
		else
		{
			return $this->item->format("<h4><a {$href}>{title}</a></h4>{$details}");
		}
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
	
	function verifyEnabled($article_id)
	{
		if (!$article_id) return false;

		try
		{
			$article = $this->Article();
			// returns null if no related blog
			$blog = $article->getBlog();
				
			return ($blog) ? $blog->allow_comments : $article->allow_comment;
		}
		catch(DataNotFoundException $e)
		{
			return false;
		}	
	}
	
	function getURL()
	{
		if(!$this->article_id) return "";
		
		$url = $this->Article()->getUrl();
		
		return $url . $this->format("#comment{comment_id}");
	}
	
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


class ArticleSolrAdapter // Implements ISolrAdapter
{

	function getClass()
	{
		return Article;
	}

	function getFilter()
	{
		return "WHERE published=1";
	}

	function getTitleFormat()
	{
		return "{title:xml}";
	}

	function getContentFormat()
	{
		return "{teaser:xml} {message:xml}";
	}

	function getAuthorFormat()
	{
		return "{author:xml}";
	}

	function getKeywordFormat()
	{
		return "{tags:xml}";
	}
	
	function wrap($item)
	{
		return new ArticleSearchResult($item);
	}
}

?>
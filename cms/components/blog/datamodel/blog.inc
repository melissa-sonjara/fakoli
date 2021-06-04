<?php
/**\addtogroup blog
 * @{
 */


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
Fakoli::using("search");

class Blog extends DataItem // implements Searchable
{
	// Fields
	var $fields = array("blog_id"				=>	Number,
						"title"					=>	String,
						"identifier"			=>	String,
						"image_id"				=>	Number,
						"description"			=>	HTML,
						"owner_id"				=>	Number,
						"read_access"			=>	String,
						"write_access"			=>	String,					
						"default_article_order"	=>	String,
						"articles_per_page"		=>	Number,
						"blog_type"				=>	String,
						"allow_comments"		=>	Boolean,
						"comment_moderators"	=>	String,
						"published"				=>	Boolean,
						"created_date"			=>	DateTime,
					    "allow_subscriptions"	=>	Boolean,
						"enable_rss_feed"		=>	Boolean,
						"max_rss_articles"		=>	Number);
	
	var $versioned_fields = array("title", "description", "image_id");
	
	// Relations
	
	var $relations = array(
			"Owner"			=>	"",
			"Articles"		=>	Article,
			"Image"			=>	ImageRecord,
			"Subscribers"	=>	BlogSubscriber
			);
					
	function Owner()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->owner_id);
	}
	
	function getOwnerName()
	{
		return $this->Owner()->getFullName();
	}
		
	function Image()
	{
		return $this->getRelated(ImageRecord);
	}
	
	function Subscribers($constraint = "")
	{
		return $this->getRelatedList(BlogSubscriber, "", $constraint);
	}
	
	function allowDelete()
	{
		if(!checkRole("admin"))
		{
			return false;
		}
		
		$articles = Query::create(Article, "WHERE article_type=:article_type")
			->bind(":article_type", $this->title)
			->executeValue("COUNT(1)");
		
		return $articles > 0 ? false : true;
	}
	
	function getSortOrderConstraint()
	{
		$constraint = "";
		switch($this->default_article_order)
		{
			case "DESC":
				$constraint = " ORDER BY publish_date DESC";
				break;
				
			case "ASC":
				$constraint = " ORDER BY publish_date ASC";
				break;
				
			case "SORT":
			default:
				$constraint = " ORDER BY sort_order";
				break;
		}
		
		return $constraint;
	}
	
	function Articles($constraint = "")
	{
		if (!$constraint || stripos($constraint, "ORDER") === false)
		{
			$constraint .= $this->getSortOrderConstraint();
		}

		$constraint = preg_replace("/^\s*WHERE\s+/i", "AND ", $constraint);
	
		$constraint = "WHERE article_type=:a $constraint";
		
		$articles = Query::create(Article, $constraint)
					->bind(":a", $this->title)
					->execute();
					
		return $articles;
	}
		
	function countArticles()
	{
		return Query::create(Article, "WHERE article_type=:a")
					->bind(":a", $this->title)
					->executeValue("COUNT(1)");
	}

	static function findByIdentifier($identifier)
	{
		$match = Query::create(Blog, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
	}	
	
	static function findByTitle($title)
	{
		$match = Query::create(Blog, "WHERE title=:title")
					  ->bind(":title", $title)
					  ->executeSingle();

		return $match;
	}
	

	/**
	 * Determines the URL for the blog
	 */
	function getURL()
	{
		global $config;
		
		$url = "";
		
		if ($this->identifier)
		{
			$contents = SectionContent::getMappedContent($this->identifier);
			if (count($contents))
			{
				$section = $contents[0]->Section();
				
				$url = $section->getSectionURL() . $this->identifier;
			}
		}
		
		return $url;
	}
	
	static function getArticleTypeOptions()
	{
		$blogs = Query::create(Blog, "ORDER BY title")->execute();
		$options = array();
		foreach($blogs as $blog)
		{
			$options[$blog->title] = $blog->title;
		}
		
		return $options;
	}
		
		
	function enumerateContentLinks()
	{
		$articles = $this->Articles("WHERE published=1 ORDER BY sort_order, title");
		$links = array();
		foreach($articles as $article)
		{
			$links[$article->title] = $this->identifier."?article_id=".$article->article_id;
		}
		
		return $links;
	}
	
	function Blog()
	{
		$this->primary_key = "blog_id";
		$this->table = "blog";
		
		$this->default_format = "{title}";
		
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 		
		
		$this->DataItem(func_get_args());
	}

	// Searchable
	
	function search($params, $range = null)
	{
		trace("Searching Blogs", 3);
		
		if ($range)
		{
			$range = " AND {$this->primary_key} IN (".implode($range, ", ").")";
		}
		
		if (is_object($params))
		{
			$search = clone $params;
			$search->target = $this;
			
			$search->remapField("text", "description");
			
			$constraint = $search->generateConstraint();
			$constraint .= $constraint ? " AND published=1" : " WHERE published=1";
			
			$constraint .= $range;
			
			$blogs = Query::create(Blog, $constraint)->execute();
		}
		else
		{
			$params = "%$params%";
			$blogs = Query::create(Blog, "WHERE (title like :a OR description LIKE :b) AND published=1 $range")
						  ->bind(":a", $params, ":b", $params)
						  ->execute();
		}
							  
		return SearchManager::wrap($blogs, BlogSearchResult);
	}
}

class BlogSearchResult extends AbstractSearchResult
{
	var $item;
	
	function BlogSearchResult($item)
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
		if(Settings::getValue("search", "show_text_fragment"))
		{
			return $this->item->format("<h4>{title}</h4><p>{description:200}</p><a href='{getURL()}'>Read More</a>");
		}
		else
		{
			return $this->item->format("<h4><a href='{getURL()}'>{title}</a></h4>");
		}
	}
}


class BlogSearchAdapter // Implements ISolrAdapter
{

	function getClass()
	{
		return Blog;
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
		return "{description:xml}";
	}

	function getAuthorFormat()
	{
		return "{Owner.last_name:xml}";
	}

	function getKeywordFormat()
	{
		return "";
	}

	function wrap($item)
	{
		return new BlogSearchResult($item);
	}
}
/**@}*/?>
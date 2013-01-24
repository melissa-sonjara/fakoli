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

class Blog extends DataItem
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
						"published"				=>	Boolean,
						"created_date"			=>	DateTime,
						"enable_rss_feed"		=>	Boolean,
						"max_rss_articles"		=>	Number);
	
	// Relations
	
	var $relations = array("Owner"		=>	"",
						   "Articles"	=>	Article,
						   "Image"		=>	ImageRecord);
					
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
	
	function Articles($constraint = "")
	{
		if (!$constraint || stripos($constraint, "ORDER") === false)
		{
			switch($this->default_article_order)
			{
				case "DESC":
					$constraint .= " ORDER BY publish_date DESC";
					break;	
								
				case "ASC":
					$constraint .= " ORDER BY publish_date ASC";
					break;
					
				case "SORT":
				default: 
					$constraint .= " ORDER BY sort_order";
					break;
			}
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
}

?>
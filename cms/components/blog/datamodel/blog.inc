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
						"blog_type"				=>	String,
						"allow_comments"		=>	Boolean,
						"published"				=>	Boolean,
						"created_date"			=>	DateTime);
	
	// Relations
	
	var $relations = array("Owner"		=>	"",
						   "Articles"	=>	Article,
						   "Image"		=>	Images);
						
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
		return $this->getRelated(Image);
	}
	
	function Articles($constraint = "")
	{
		if (!$constraint)
		{
			$constraint = "ORDER BY created_date $this->default_article_order";
		}
		else
		{
			$constraint = preg_replace("/^\s*WHERE\s+/i", "AND ", $constraint);
		}
		
		$constraint = "WHERE article_type=:a $constraint";
		
		$articles = Query::create(Article, $constraint)
					->bind(":a", $this->title)
					->execute();
					
		return $articles;
	}
	
	function countArticles()
	{
		return queryValue(Article, "COUNT(1)", "WHERE article_type='{$this->title}'");
	}

	static function findByIdentifier($identifier)
	{
		$match = Query::create(Blog, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
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
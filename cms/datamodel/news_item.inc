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
 
require_once realpath(dirname(__FILE__)."/../../framework/data_item.inc");
require_once realpath(dirname(__FILE__)."/image.inc");
/**
 * Defines the NewsItem class.
 */
class NewsItem extends DataItem
{
	var $fields = array("news_item_id"	=>	Number,
						"title"			=>	String,
						"image_id"		=>	Number,
						"article_type"	=>	String,
						"teaser"		=>	HTML,
						"message"		=>	HTML,
						"created_date"	=>	Date,
						"archive_date"  =>  Date,
						"last_modified"	=>	Timestamp,
						"published"		=>	Boolean);

	var $relations = array(	"Image" => Image,
							"Sites"	=>	Site,
							"Comments" => Comment);
	
	function Image()
	{
		return $this->getRelated(Image);
	}
	
	function Sites($constraint = "")
	{
		return $this->crossReference(Site, NewsItemSiteXref, $constraint);
	}
	
	function Comments($constraint = "")
	{
		return $this->crossReference(Comment, NewsItemCommentXref, $constraint);
	}
	
	function NewsItem()
	{
		$this->table = "news_item";
		$this->primary_key = "news_item_id";
		$this->pretty_class_name = "News Item";
		
		$this->DataItem(func_get_args());
	}
}

class NewsItemSiteXref extends DataItem
{
	var $fields = array("news_item_site_xref_id"	=>	Number,
						"news_item_id"				=>	Number,
						"site_id"					=>	Number);
	
	var $relations = array(	"NewsItem"	=>	NewsItem,
							"Site"		=>	Site);
	
	function NewsItem()
	{
		return $this->getRelated(NewsItem);
	}
	
	function Site()
	{
		return $this->getRelated(Site);
	}
	
	function NewsItemSiteXref()
	{
		$this->table = "news_item_site_xref";
		$this->primary_key = "news_item_site_xref_id";
		$this->DataItem(func_get_args());
	}
}

class NewsItemCommentXref extends DataItem
{
	var $fields = array("news_item_comment_xref_id"	=>	Number,
						"news_item_id"				=>	Number,
						"comment_id"					=>	Number);
	
	var $relations = array(	"NewsItem"	=>	NewsItem,
							"Comment"		=>	Comment);
	
	function NewsItem()
	{
		return $this->getRelated(NewsItem);
	}
	
	function Comment()
	{
		return $this->getRelated(Comment);
	}
	
	function NewsItemCommentXref()
	{
		$this->table = "news_item_comment_xref";
		$this->primary_key = "news_item_comment_xref_id";
		$this->DataItem(func_get_args());
	}
}

?>
<?php
/**************************************************************

Copyright (c) 2010 Sonjara, Inc

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated linkation
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

/**
 * This class maps the publication table.
*/
class LinkRecord extends DataItem // implements Searchable
{
	var $table = "link_record";
	var $primary_key = "link_id";
	var $pretty_class_name = "Link";
	
	// Fields

	var $fields = array("link_id"			=>	Number,
						"title"				=>	String,
						"url"				=>	String,
						"description"		=>	HTML,
						"published"			=>	Boolean,
						"last_modified"		=>	Timestamp,
						"link_library_id"	=>	Number,
						"owner_id"			=>	Number,
						"sort_order"		=>	Number);

	// Relations

	var $relations = array("Owner"			=>	"",
						   "LinkLibrary"	=>	LinkLibrary
	);

	function Owner()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->owner_id);
	}

	function isOwner()
	{
		global $user;
		$owner = $this->Owner();

		return ($owner->user_id == $user->user_id) ? true : false;
	}
	
	function LinkLibrary()
	{
		return $this->getRelated(LinkLibrary);
	}
	
	function search($params, $range = null)
	{
		trace("Searching Links", 3);

		if ($range)
		{
			$range = " AND {$this->primary_key} IN (".implode($range, ", ").")";
		}
		
		if (is_object($params))
		{
			$search = clone $params;
			$search->target = $this;
			
			if (!$range)
			{
				$search->remapField("text", "description");
			}
			
			$constraint = $search->generateConstraint();
			$constraint .= $constraint ? " AND published=1" : " WHERE published=1";
			
			$constraint .= $range;
			
			$links = Query::create(LinkRecord, $constraint)
						  ->execute();
		}
		else
		{
			$params = preg_replace('/[^\w]+/', ' ', $params);
			$params = "%$params%";
			$links = Query::create(LinkRecord, "WHERE (title like :a OR description Like :b) and published=1 $range")
						  ->bind(":a", $params, ":b", $params)
						  ->execute();
		}
			
		return SearchManager::wrap($links, LinkSearchResult);
	}
}

class LinkSearchResult extends AbstractSearchResult
{
	var $item;
	
	function LinkSearchResult($item)
	{
		$this->item = $item;
	}

	function getPrimaryKey() { return $this->item->getPrimaryKey(); }
	function get($field) { return $this->item->get($field); }
	function prettifyClassName($plural = false) { return $this->item->prettifyClassName($plural = false); }
	function format($format) { return $this->item->format($format); }
	
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
		return $this->item->publication_date;
	}
	
	function summary()
	{
		$url = $this->item->url;
		
		$img = $this->formatIcon(getDocIcon($url), getDocType($url));
		
		return $this->item->format("{$img}<h4><a href='{url}' target='_blank'>{title}</a></h4><p><em>{description}</em></p>");
	}
}

?>
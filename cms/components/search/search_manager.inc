<?php
/**************************************************************

 Copyright (c) 2011 Sonjara, Inc

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

Fakoli::using("search", "component");
Fakoli::usingFeature("paged_list");
/**
 * The SearchManager class provides a simple API for searching across
 * all Searchable records in the application database.
 * @author andy
 *
 */
class SearchManager
{
	static $map;
	
	var $searchResults;
	
	function SearchManager($results = null)
	{
		$this->searchResults = $results;
	}
	
	static function upgradeComponent($version)
	{
		Cache::invalidate("fakoli_search_map");
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("search", "search_result_format", "Combined List", "String", "Specifies the display format to use when displaying results", "", "Combined List\nGrouped by Type");
		Settings::setDefaultValue("search", "results_per_page", "20", "Number", "Specifies the number of results to show per page. Set to zero for no pagination.");
		Settings::setDefaultValue("search", "show_text_fragment", false, "Boolean", "Specifies whether to show with the search results. Note that not all search types will return a text fragment"); 
		
		SearchManager::buildSearchMap();
	}
	
	static function buildSearchMap()
	{
		if (!SearchManager::$map)
		{
			SearchManager::$map = Cache::get("fakoli_search_map");
		}
		
		if (!SearchManager::$map)
		{
			$map = array();
			$map = ComponentManager::fireEvent("RegisterSearchables", $map);
			Cache::put("fakoli_search_map", $map);
			SearchManager::$map = $map;
		}
	}
	
	
	function search($params)
	{
		$searchResults = array();
		
		SearchManager::buildSearchMap();
		
		if (is_object($params))
		{
			$class_array = $_REQUEST["searchable_classes:"];
		}
		else
		{
			$params = trim($params);
		}
		
		if (!$params)
		{
			$this->searchResults = array();
			return this;
		}
		
		foreach(SearchManager::$map as $component => $searchables)
		{
			Fakoli::using($component);
			
			foreach($searchables as $searchable)
			{
				if ($class_array && !array_key_exists($searchable, $class_array)) continue;
				
				$obj = new $searchable;
				$results = $obj->search($params);
				$searchResults = array_merge($searchResults, $results);
			}
		}
		
		$this->searchResults = $searchResults;
		return $this;
	}
	
	function sortByRelevance()
	{
		usort($this->searchResults, array(SearchManager, compareRelevance));
		return $this;
	}
	
	static function compareRelevance($item1, $item2)
	{
		return ($item1->relevance() - $item2->relevance());
	}
	
	function sortByTitle()
	{
		usort($this->searchResults, array(SearchManager, compareTitle));
		return $this;
	}
	
	static function compareTitle($item1, $item2)
	{
		return strcmp($item1->title(), $item2->title());
	}
	
	function sortByDate()
	{
		usort($this->searchResults, array(SearchManager, compareDate));
		return $this;
	}
	
	static function compareDate($item1, $item2)
	{
		return strtotime($item2->date()) - strtotime($item1->date());
	}
	
	static function formatSearchResult($item)
	{
		return $item->summary();
	}
	
	function showResults($pageSize = 0)
	{
		global $script;
		
		if ($pageSize == 0) $pageSize = Settings::getValue("search", "results_per_page");
		
		$list = new PagedList($this->searchResults, "search_results", array(SearchManager, formatSearchResult));
		$list->pageSize = $pageSize;
		$list->emptyList = "Your search did not match any items.";
		
		$script .= $list->writeScript();
		
		$num = count($this->searchResults);
		
		if ($num)
		{
			echo "<p style='float: left'><em>$num ".pluralize("match", $num)." found.</em></p>";
		}
		
		$list->drawList();
	}
	
	static function wrap($items, $resultClass)
	{
		$results = array();

		if (is_array($items))
		{
			foreach($items as $item)
			{
				$results[] = new $resultClass($item);
			}
		}
				
		return $results;
	}
}

?>
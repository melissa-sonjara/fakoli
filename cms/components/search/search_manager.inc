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

Fakoli::using("search", "component", "taxonomy");
Fakoli::usingFeature("paged_list", "facet_manager");
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
	var $useFacetFilter = false;
	var $displayMode = "list";
	
	// Facet dimensions
	var $facetDropdown = true;
	var $facetMaxWidth = "200px";
	var $facetWidth = "200px";
	var $facetHeight = "120px";
	
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
		}
		else
		{
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
		}

		switch($this->displayMode)
		{
			case "table":
				$this->list = new DataListView($this->extractDataItems(), "search_results");
				$this->list->emptyMessage = "Your search did not match any items.";
				$this->list->writeIdTag = true;
				$this->list->sortable = true;
				$this->list->filter = false;
				break;
				
			case "list":
			default:
				$this->list = new PagedList($this->searchResults, "search_results", array(SearchManager, formatSearchResult));
				$this->list->emptyList = "Your search did not match any items.";
				$this->list->writeIdTag = true;
		}
		
		if ($this->enableFacetFilter)
		{
			$this->facetManager = new FacetManager('facet_manager', $this->list);
			$classes = $this->getSearchableClasses();
			trace("Facet Classes: ".implode(", ", $classes), 3);				
			TaxonomyManager::addFacets($this->facetManager, $classes, $this->facetDropdown, $this->facetMaxWidth, $this->facetWidth, $this->facetHeight);
		}
		
		return $this;
	}
	
	function getSearchableClasses()
	{
		if (isset($_REQUEST["searchable_classes:"]))
		{
			$class_array = $_REQUEST["searchable_classes:"];
		}
		
		$cl = array();
		
		foreach(SearchManager::$map as $component => $searchables)
		{		
			foreach($searchables as $searchable)
			{
				if ($class_array && !array_key_exists($searchable, $class_array)) continue;
				$cl[] = $searchable;
			}
		}
		
		return $cl;
	}
	
	function extractDataItems()
	{
		$items = array();
		
		foreach($this->searchResults as $result)
		{
			$items[] = $result->item;
		}
		
		return $items;
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
	
	function drawFacets($id = null)
	{
		global $script;
		if ($this->drawnFacets || !$this->facetManager) return;
		
		if ($id)
		{
			$this->facetManager->id = $id;
		}
		
		$script .= $this->facetManager->writeScript();
		echo $this->facetManager->drawForm();
		
		$this->drawnFacets = true;
	}
	
	function showResults($pageSize = 0)
	{
		global $script;
		
		if ($pageSize == 0) $pageSize = Settings::getValue("search", "results_per_page");
		
		$this->list->pageSize = $pageSize;
		
		$script .= $this->list->writeScript();
		
		$num = count($this->searchResults);
		
		if ($num)
		{
			echo "<p><em>$num ".pluralize("match", $num)." found.</em></p>";
		}
		
		$this->drawFacets();
		
		switch($this->displayMode)
		{
			case "table":
				$this->list->drawView();
				break;
				
			case "list":
			default:
				$this->list->drawList();
				break;
		}
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
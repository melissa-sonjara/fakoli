<?php
/**\addtogroup page
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

Fakoli::usingFile("framework/join.inc");
Fakoli::using("site", "search");

class Page extends DataItem implements Searchable
{
	// Fields
	var $fields = array("page_id" 				=>	Number,
						"identifier"			=>	String,
						"page_title"			=>	String,
						"description"	 		=>	HTML,
						"site_id"				=>  Number,
						"template"				=>	String,
						"meta_tag_description"	=>	Text,
						"meta_tag_keyword"		=>	String,
						"php_code_file"			=>	String,
						"role"  				=> 	String,
						"published"				=> 	Boolean,
						"exclude_from_search"	=>	Boolean,
						"created_date" 			=>	Date,
						"edited_date"			=>	Timestamp,
						"author" 				=>  String
	);
	
	// Relations
	
	var $relations = array("PageModuleXrefs"	=>	PageModuleXref,
						   "Modules"			=>	Module,
						   "Site"				=>	Site);
	
	var $versioned_fields = array("page_title", "description", "meta_tag_description", "meta_tag_keyword", "author");
	
	function PageModuleXrefs($constraint = "ORDER BY position, sort_order")
	{
		return $this->getRelatedList(PageModuleXref, "", $constraint);
	}
	
	function Modules($constraint = "ORDER BY position, sort_order")
	{
		return $this->crossReference(Module, PageModuleXref, $constraint);
	}
	
	function Site()
	{
		return $this->getRelated(Site);
	}
	
	static function findByIdentifier($identifier)
	{
		$match = Query::create(Page, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
	}	
	
	function getModulesByPosition($constraint = "ORDER BY position, sort_order")
	{
		global $section;
		global $identifier;
		
		$join = new InnerJoin();
		$join->add(Module);
		$join->add(PageModuleXref);
		
		$result = $join->groupedQuery("WHERE page_id={$this->page_id} $constraint", "PageModuleXref.position");
		$modules = extractGroupedJoinResults(Module, $result);

		// If no modules have been set, use the section defaults
		if (count($modules) == 0 && $section)
		{
			$content = $section->getContent($identifier);
			if ($content) $modules = $content->getModulesByPosition($constraint);
		}
		
		if (count($modules) == 0 && $section)
		{
			$modules = $section->getModulesByPosition($constraint);
		}
		
		// Merge in global modules
		$globals = groupedQuery(Module, "WHERE global=1", "global_position");
		foreach($globals as $position => $modulesAtPosition)
		{
			if (array_key_exists($position, $modules))
			{
				$modules[$position] = removeDuplicates(array_merge($modulesAtPosition, $modules[$position]));
			}
			else
			{
				$modules[$position] = $modulesAtPosition;
			}
		}
		
		return $modules;
	}
	
	
	/**
	 * Loads the template associated with this page.
	 * @return string the contents of the template for this page.
	 */
	function getTemplate()
	{
		global $config;
		global $section;
		
		$style = $_REQUEST["_style"];
		$site = $this->Site();
		if (!$site)
		{
			$site = Site::getSite();
		}
		
		$templateFile = "";
		
		switch($style)
		{
		case 'print':
			$templateFile = $site->print_template;
			break;

		case 'popup':
			$templateFile = $site->popup_template;
			break;
			
		case 'mobile':
			$templateFile = $site->mobile_template;
			
		case 'nude':
			return "<h2>{page_title}</h2>{description}";
		}
		
		if ($section)
		{
			if (!$templateFile) $templateFile = $section->getTemplateFile($this->identifier);
		}

		if (!$templateFile) $templateFile = $this->template;
		if (!$templateFile) $templateFile = $site->default_template;
				
		$templateFile = ComponentManager::fireEvent("OverrideTemplate", $templateFile);

		$base = $site->getThemeDirectory();
		$template = file_get_contents("{$base}/{$templateFile}");

		return $template;
	}
	
	/**
	 * Retrieves an array of the position names defined in the template for this page.
	 * @return array the positions defined in the template (sorted alphabetically)
	 */
	function getPositions()
	{
		$template = $this->getTemplate();
		
		$positionMatches = array();
		
		preg_match_all("/\{position:(.*?)\}/", $template, $positionMatches, PREG_PATTERN_ORDER);
		
		$positions = $positionMatches[1];		
		sort($positions);
		return $positions;
	}

	function getBodyClass()
	{
		if (Fakoli::$bodyClass) return Fakoli::$bodyClass;
		
		global $section;
		if ($section)
		{
			$content = $section->getContent($this->identifier);
			if ($content) return $content->body_class ? $content->body_class : $section->default_body_class;
		}
	
		return "";
	}
		
	/*var $relations = array(	"Region"	=> 	Region,
							"Users"		=>	SiteUser);
	
	function Region()
	{
		return $this->getRelated(Clinic);
	}

	function Users()
	{
		return $this->getRelatedList(SiteUser);
	}*/
	
	// Constructor
	function __construct()
	{
		$this->primary_key = "page_id";
		$this->table = "page";
		
		$this->default_format = "{page_title}";
		
		parent::__construct(func_get_args());
	}
	
	function search($params, $range = null)
	{
		trace("Searching Pages", 3);
		
		if ($range)
		{
			$range = " AND {$this->primary_key} IN (".implode($range, ", ").")";
		}
		
		if (is_object($params))
		{
			$search = clone $params;
			$search->target = $this;
			$search->remapField("title", "page_title");
			$search->remapField("keywords", "meta_tag_keyword");			
			$search->remapField("publication_date", "created_date");
			
			if (!$search->get("text", "like"))
			{
				$search->secondaryFields("meta_tag_keyword", "description");
			}

			
			$constraint = $search->generateConstraint();
			$constraint .= $constraint ? " AND published=1 AND exclude_from_search=0" : " WHERE published=1 AND exclude_from_search=0";
			$constraint .= $range;
			
			$pages = Query::create(Page, $constraint)
						  ->execute();
		}
		else
		{
			$params = "%$params%";
			$pages = Query::create(Page, "WHERE (page_title like :a OR description LIKE :b) AND published=1 AND exclude_from_search=0 $range")
						  ->bind(":a", $params, ":b", $params)
						  ->execute();
		}
							  
		return SearchManager::wrap($pages, PageSearchResult);
	}
}

class PageSearchResult extends AbstractSearchResult
{
	var $item;
	
	function __construct($item)
	{
		$this->item = $item;
	}

	function getPrimaryKey() { return $this->item->getPrimaryKey(); }
	function get($field) { return $this->item->get($field); }
	function prettifyClassName($plural = false) { return $this->item->prettifyClassName($plural = false); }
	function format($format) { return $this->item->format($format); }
	
	function relevance()
	{
		return 0.2;
	}
	
	function title()
	{
		return $this->item->page_title;
	}
	
	function date()
	{
		return $this->item->created_date;
	}
		
	function summary()
	{		
		$img = $this->formatIcon('/fakoli/richtext/images/hyperlink.gif', 'cms_page');
		
		if(Settings::getValue("search", "show_text_fragment"))
		{
			return $this->item->format("{$img}<h4>{page_title}</h4><p>{description:300}</p><a href='{identifier}'>Read More</a>");
				
		}
		else
		{
			return $this->item->format("{$img}<h4><a href='{identifier}'>{page_title}</a></h4>");
		}
	}
}

class PageModuleXref extends DataItem
{
	// Fields
	var $fields = array("join_id"		=>	Number,
						"page_id"		=>	Number,
						"module_id"		=>	Number,
						"position"		=>	String,
						"sort_order"	=>	Number);
	
	// Relations
	
	var $relations = array(	"Page"		=>	Page,
							"Module"	=>	Module);
	
	function Page()
	{
		return $this->getRelated(Page);
	}
	
	function Module()
	{
		return $this->getRelated(Module);
	}
	
	function __construct()
	{
		$this->table = "page_module_xref";
		$this->primary_key = "join_id";
		
		parent::__construct(func_get_args());
	}
}

class PageSolrAdapter // Implements ISolrAdapter
{

	function getClass()
	{
		return Page;
	}
	
	function getFilter()
	{
		return "WHERE published=1 AND exclude_from_search=0";
	}
	
	function getTitleFormat()
	{
		return "{page_title:xml}";
	}
	
	function getContentFormat()
	{
		return "{description:xml}";
	}
	
	function getAuthorFormat()
	{
		return "{author:xml}";
	}
	
	function getKeywordFormat()
	{
		return "{meta_tag_keyword:xml}";
	}
	
	function wrap($item)
	{
		return new PageSearchResult($item);
	}
}
/**@}*/?>

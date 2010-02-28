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
require_once realpath(dirname(__FILE__)."/../../framework/join.inc");
require_once realpath(dirname(__FILE__)."/site.inc");

class Page extends DataItem
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
						"created_date" 			=>	Date,
						"edited_date"			=>	Timestamp,
						"author" 				=>  String
	);
	
	// Relations
	
	var $relations = array("PageModuleXrefs"	=>	PageModuleXref,
						   "Modules"			=>	Module,
						   "Site"				=>	Site);
	
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
	
	function getModulesByPosition($constraint = "ORDER BY position, sort_order")
	{
		$join = new InnerJoin();
		$join->add(Module);
		$join->add(PageModuleXref);
		
		$result = $join->groupedQuery("WHERE page_id={$this->page_id} $constraint", "PageModuleXref.position");
		return extractGroupedJoinResults(Module, $result);
	}
	
	
	/**
	 * Loads the template associated with this page.
	 * @return string the contents of the template for this page.
	 */
	function getTemplate()
	{
		global $config;
		$style = $_REQUEST["_style"];
		$site = $this->Site();
		
		$templateFile = "";
		
		switch($style)
		{
		case 'print':
			$templateFile = $site->print_template;
			break;

		case 'popup':
			$templatefile = $site->popup_template;
			break;
			
		case 'mobile':
			$templateFile = $site->mobile_template;
		}
		
		if (!$templateFile) $templateFile = $this->template;
		
		$template = file_get_contents("{$config['homedir']}/templates/{$templateFile}");

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
	function Page()
	{
		$this->primary_key = "page_id";
		$this->table = "page";
		
		$this->default_format = "{title}";
		
		$this->DataItem(func_get_args());
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
	
	function PageModuleXref()
	{
		$this->table = "page_module_xref";
		$this->primary_key = "join_id";
		
		$this->DataItem(func_get_args());
	}
}
?>
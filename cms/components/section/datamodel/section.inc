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

class Section extends DataItem
{
	// Fields
	
	var $fields = array("section_id"			=>	Number,
						"section"				=>	String,
						"section_title"			=>	String,
						"default_template"		=>	String,
						"default_role"			=>	String,
						"default_permissions"	=>	String,
						"default_page"			=>	String,
						"site_id"				=>	Number,
						"use_SSL"				=>	Boolean);
	
	var $relations = array("SectionModuleXrefs"	=>	SectionModuleXref,
						   "Modules"			=>	Module,
						   "Content"			=>	SectionContent,
						   "Site"				=>	Site
							);
	
	function SectionModuleXrefs($constraint = "ORDER BY position, sort_order")
	{
		return $this->getRelatedList(SectionModuleXref, "", $constraint);
	}
	
	function Modules($constraint = "ORDER BY position, sort_order")
	{
		return $this->crossReference(Module, SectionModuleXref, $constraint);
	}
	
	function Content($constraint = "")
	{
		return $this->getRelatedList(SectionContent, "", $constraint);
	}
		
	function Site()
	{
		return $this->getRelated(Site);
	}
	
	static function createFromIdentifier($identifier)
	{
		if (!$identifier) return null;
		
		try
		{
			return Query::create(Section, "WHERE section=:identifier")
						->bind(":identifier", $identifier)
						->executeSingle();
		}
		catch(Exception $e)
		{
			throw new FakoliException("Unknown or ambiguous section '$identifier'");
		}		
	}
	
	function getContent($identifier)
	{
		try
		{
			return Query::create(SectionContent, "WHERE section_id=:section_id AND identifier=:identifier")
						->bind(":section_id", $this->section_id, ":identifier", $identifier)
						->executeSingle();
		}
		catch(DataNotFoundException $e)
		{
			return null;
		}
	}
	
	function getTemplateFile($identifier)
	{
		$content = is_object($identifier) ? $identifier : $this->getContent($identifier);
		if ($content->template) return $content->template;
		return $this->default_template;
	}
	
	function getRole($identifier)
	{
		$content = is_object($identifier) ? $identifier : $this->getContent($identifier);
		if ($content->role) return $content->role;
		return $this->default_role;
	}
	
	function getPermissions($identifier)
	{
		$content = is_object($identifier) ? $identifier : $this->getContent($identifier);
		if ($content->permissions) return $content->permissions;
		return $this->default_permissions;
	}
	
	static function findSection($section_name)
	{
		$sections = Query::create(Section, "WHERE section=:section")
			->bind(":section", $section_name)
			->execute();
		
		return (count($sections) > 0) ? $sections[0] : null;
	}
	
	function getModulesByPosition($constraint = "ORDER BY position, sort_order")
	{
		$modules = Cache::get("section_module_map_{$this->section_id}");
		if (!$modules)
		{
			$join = new InnerJoin();
			$join->add(Module);
			$join->add(SectionModuleXref);
			
			$result = $join->groupedQuery("WHERE section_id={$this->section_id} $constraint", "SectionModuleXref.position");
			$modules = extractGroupedJoinResults(Module, $result);
			
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
			
			Cache::put("section_module_map_{$this->section_id}", $modules);
		}
				
		return $modules;
	}
	
	function clearModuleCache()
	{
		Cache::invalidate("section_module_map_{$this->section_id}");
	}
	
	/**
	 * Returns the Section URL
	 */
	
	function getSectionURL()
	{
		global $config;
	
		$url = "http" . (($this->use_SSL) ? "s" : "") . "://{$config['http_host']}/";
		if ($this->section != "/")
		{
			$url .= $this->section . "/";
		}
		return $url;
	}
	
	/**
	 * Loads the default template associated with this section.
	 * @return string the contents of the template for this section.
	 */
	function getDefaultTemplate()
	{
		global $config;
		$template = file_get_contents("{$config['homedir']}/templates/{$this->default_template}");

		return $template;
	}
	
	/**
	 * Retrieves an array of the position names defined in the template for this page.
	 * @return array the positions defined in the template (sorted alphabetically)
	 */
	function getPositions()
	{
		$template = $this->getDefaultTemplate();
		
		$positionMatches = array();
		
		preg_match_all("/\{position:(.*?)\}/", $template, $positionMatches, PREG_PATTERN_ORDER);
		
		$positions = $positionMatches[1];		
		sort($positions);
		return $positions;
	}

	
	function Section()
	{
		$this->primary_key = "section_id";
		$this->table = "section";
		
		$this->DataItem(func_get_args());
	}
}

class SectionContent extends DataItem
{
	// Fields
	
	var $fields = array("section_content_id"	=>	Number,
						"section_id"			=>	String,
						"identifier"			=>	String,
						"role"					=>	String,
						"permissions"			=>	String,
						"template"				=>	String,
						"use_SSL"				=>	Boolean);
	
	var $relations = array(
						"Section"	=>	Section
	);
	

	function Section()
	{
		return $this->getRelated(Section);
	}
	

	static function getMappedContent($identifier)
	{
		$content = Query::create(SectionContent, "WHERE identifier=:i")
						->bind(":i", $identifier)
						->execute();
		
		return $content;
	}
	
	function SectionContent()
	{
		$this->primary_key = "section_content_id";
		$this->table=  "section_content";
		
		$this->DataItem(func_get_args());
	}
}


class SectionModuleXref extends DataItem
{
	// Fields
	var $fields = array("join_id"		=>	Number,
						"section_id"	=>	Number,
						"module_id"		=>	Number,
						"position"		=>	String,
						"sort_order"	=>	Number);
	
	// Relations
	
	var $relations = array(	"Section"	=>	Section,
							"Module"	=>	Module);
	
	function Section()
	{
		return $this->getRelated(Section);
	}
	
	function Module()
	{
		return $this->getRelated(Module);
	}
	
	function SectionModuleXref()
	{
		$this->table = "section_module_xref";
		$this->primary_key = "join_id";
		
		$this->DataItem(func_get_args());
	}
}

?>
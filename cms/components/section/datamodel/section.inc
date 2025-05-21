<?php
/**\addtogroup section
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

/**
 * Section DataItem, defining the data model for sections within
 * a site.
 * @author Andy Green
 */
class Section extends DataItem
{
	var $primary_key = "section_id";
	var $table = "section";
	
	// Fields
	
	var $fields = array("section_id"			=>	'Number',
						"section"				=>	'String',
						"section_title"			=>	'String',
						"default_template"		=>	'String',
						"default_role"			=>	'String',
						"default_permissions"	=>	'String',
						"default_page"			=>	'String',
						"default_body_class"	=>	'String',
						"site_id"				=>	'Number',
						"use_SSL"				=>	'Boolean',
						"section_type"			=>	'String'
	);
	
	var $relations = array("SectionModuleXrefs"	=>	'SectionModuleXref',
						   "Modules"			=>	'Module',
						   "Content"			=>	'SectionContent',
						   "Site"				=>	'Site'
							);
	
	function SectionModuleXrefs($constraint = "ORDER BY position, sort_order")
	{
		return $this->getRelatedList('SectionModuleXref', "", $constraint);
	}
	
	function Modules($constraint = "ORDER BY position, sort_order")
	{
		return $this->crossReference('Module', 'SectionModuleXref', $constraint);
	}
	
	function Content($constraint = "")
	{
		return $this->getRelatedList('SectionContent', "", $constraint);
	}
		
	function Site()
	{
		return $this->getRelated('Site');
	}
	
	function getContentManager()
	{
		return SectionContentManagerMap::getManager($this);
	}
	
	static function createFromIdentifier($identifier)
	{
		if (!$identifier) return null;

		$site = Site::getSite();
		
		try
		{
			return Query::create('Section', "WHERE section=:identifier AND site_id=:site")
						->bind(":identifier", $identifier, ":site", $site->site_id)
						->executeSingle();
		}
		catch(Exception $e)
		{
			ComponentManager::fireEvent("SectionNotFound", $identifier);
			throw new FakoliException("Unknown or ambiguous section '$identifier'");
		}		
	}
	
	var $_content = array();
	
	function getContent($identifier)
	{
		if ($this->_content[$identifier]) return $this->_content[$identifier];
		SectionManager::ensureContentManager();
		
		return SectionContentManagerMap::getManager($this)->getContent($this, $identifier);
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
		$site = Site::getSite();
		
		$sections = Query::create('Section', "WHERE section=:section AND site_id=:site")
			->bind(":section", $section_name, ":site", $site->site_id)
			->execute();
		
		return (count($sections) > 0) ? $sections[0] : null;
	}
	
	function getModulesByPosition($constraint = "ORDER BY position, sort_order")
	{
		$modules = Cache::get("section_module_map_{$this->section_id}");
		if (!$modules)
		{
			$join = new InnerJoin();
			$join->add('Module');
			$join->add('SectionModuleXref');
			
			$result = $join->groupedQuery("WHERE section_id={$this->section_id} $constraint", "SectionModuleXref.position");
			$modules = extractGroupedJoinResults('Module', $result);
			
			// Merge in global modules
			$globals = groupedQuery('Module', "WHERE global=1", "global_position");
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
		$directory = $this->Site()->getThemeDirectory();
		$file = $directory . DIRECTORY_SEPARATOR . $this->default_template;
		trace("Template File: $file", 3);
		$template = file_get_contents($file);

		return $template;
	}
	
	/**
	 * Returns the path to the default page for this section
	 */
	function getDefaultPage()
	{
		$url = $this->getSectionURL();
		return $url . $this->default_page;
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
}

class SectionContent extends DataItem
{
	var $primary_key = "section_content_id";
	var $table       = "section_content";
	
	// Fields
	
	var $fields = array("section_content_id"	=>	'Number',
						"section_id"			=>	'String',
						"identifier"			=>	'String',
						"role"					=>	'String',
						"permissions"			=>	'String',
						"template"				=>	'String',
						"body_class"			=>	'String',
						"override_page_title"	=>  'String',
						"use_SSL"				=>	'Boolean');
	
	var $relations = array(
						"Section"					=>	'Section',
						"SectionContentModuleXrefs"	=>	'SectionContentModuleXref'
	);

	function Section()
	{
		return $this->getRelated('Section');
	}
	

	function SectionContentModuleXrefs($constraint = "ORDER BY position, sort_order")
	{
		return $this->getRelatedList('SectionContentModuleXref', "", $constraint);
	}

	function getModulesByPosition($constraint = "ORDER BY position, sort_order")
	{
		if (!$this->section_content_id)
		{
			// Mocked SectionContent object
			return array();
		}
		
		$modules = Cache::get("section_content_module_map_{$this->section_content_id}");
		if (!$modules)
		{
			$join = new InnerJoin();
			$join->add('Module');
			$join->add('SectionContentModuleXref');
				
			$result = $join->groupedQuery("WHERE section_content_id={$this->section_content_id} $constraint", "SectionContentModuleXref.position");
			$modules = extractGroupedJoinResults('Module', $result);
				
			if (count($modules) > 0)
			{
				// Merge in global modules
				$globals = groupedQuery('Module', "WHERE global=1", "global_position");
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
			}
							
			Cache::put("section_module_map_{$this->section_id}", $modules);
		}
	
		return $modules;
	}
	
	static function getMappedContent($identifier)
	{
		$content = Query::create('SectionContent', "WHERE identifier=:i")
						->bind(":i", $identifier)
						->execute();
		
		return $content;
	}
	

	/**
	 * Loads the default template associated with this section.
	 * @return string the contents of the template for this section.
	 */
	function getTemplate()
	{
		global $config;
		
		if ($this->template)
		{
			$template = file_get_contents("{$config['homedir']}/templates/{$this->template}");
		}
		else
		{
			$template = $this->Section()->getDefaultTemplate();
		}
		
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
	
	/**
	 * Returns any custom body CSS classes defined for this section content.
	 */
	function getBodyClass()
	{
		return $this->body_class ? $this->body_class : $this->Section()->default_body_class;
	}
	
	/**
	 * Add this SectionContent to the specified Section. This is intended for newly
	 * created SectionContent and will throw an exception if the section_id field is already
	 * populated, or if there is already a SectionContent item with the same identifier mapped to the
	 * the target section
	 * 
	 * @param string $section_folder the folder identifying the target Section
	 */
	function addToSection($section_folder)
	{
		if ($this->section_id)
		{
			throw new FakoliException("Attempt to map section content that is already mapped to a section");
		}
		
		$section = Section::findSection($section_folder);
		if (!$section)
		{
			throw new FakoliException("Unrecognized section '$section_folder'");
		}
		
		if (SectionManager::isInSection($section_folder, $this->identifier)) return;
		
		$this->section_id = $section->section_id;
		$this->save();
	}
}


class SectionModuleXref extends DataItem
{
	var $table = "section_module_xref";
	var $primary_key = "join_id";
	
	// Fields
	var $fields = array("join_id"		=>	'Number',
						"section_id"	=>	'Number',
						"module_id"		=>	'Number',
						"position"		=>	'String',
						"sort_order"	=>	'Number');
	
	// Relations
	
	var $relations = array(	"Section"	=>	'Section',
							"Module"	=>	'Module');
	
	function Section()
	{
		return $this->getRelated('Section');
	}
	
	function Module()
	{
		return $this->getRelated('Module');
	}
}



class SectionContentModuleXref extends DataItem
{
	var $table = "section_content_module_xref";
	var $primary_key = "join_id";
	
	// Fields
	var $fields = array("join_id"		=>	'Number',
						"section_content_id"	=>	'Number',
						"module_id"		=>	'Number',
						"position"		=>	'String',
						"sort_order"	=>	'Number');

	// Relations

	var $relations = array(	"SectionContent"	=>	'SectionContent',
							"Module"			=>	'Module');

	function SectionContent()
	{
		return $this->getRelated('SectionContent');
	}

	function Module()
	{
		return $this->getRelated('Module');
	}
}

/**@}*/?>

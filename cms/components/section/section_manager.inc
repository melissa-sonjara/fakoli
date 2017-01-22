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

/**
 * Handles serialization of the section map for import/export.
 * @author andy
 */
class SectionSerializationHandler
{
	function SectionSerializationHandler()
	{
	}
	
	function export()
	{
		$xml  = "\n<SectionMap>";
		$xml .= SerializationManager::serialize(Section, "ORDER BY section_id");
		$xml .= SerializationManager::serialize(SectionContent, "ORDER BY section_content_id");
		$xml .= SerializationManager::serialize(SectionModuleXref);
		$xml .= "</SectionMap>";
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(Section, $doc, $tx);
		SerializationManager::unserialize(SectionContent, $doc, $tx);
		SerializationManager::unserialize(SectionModuleXref, $doc, $tx);
	}	
}

/**
 * Maintains the map of SectionContentManagers for different Section types.
 * Components can register a custom SectionManager type by consuming the 
 * RegisterSectionManagers event and calling SectionManagerMap::registerSectionManager().
 *  
 * @author andy
 *
 */
class SectionContentManagerMap
{
	private static $map = array();
	
	/**
	 * Registers a SectionContentManager for handling a specified section type.
	 * @param string $type the type of section to be managed by the registered object
	 * @param object $manager the section manager object that will resolve content requests for sections
	 * of the given type.
	 */
	static function registerManager($type, $manager)
	{
		trace("Section Type '{$type}' will be managed by ".get_class($manager), 2);
		SectionContentManagerMap::$map[$type] = $manager;
	}
	
	/**
	 * Retrieves the SectionContentManager object for the specified section
	 * @param Section $section the Section object for which the manager is to be retrieved
	 * @return the SectionContentManager object that will resolve content for this section
	 * @throws FakoliException
	 */
	static function getManager($section)
	{
		$mgr = SectionContentManagerMap::$map[$section->section_type];
		if (!$mgr)
		{
			throw new FakoliException("Unknown section manager for section '{$section->section}'");
		}
		
		return $mgr;
	}
	
	/**
	 * Returns the list of registered section content types.
	 * @return array the list of registered section content types in alphabetical order
	 */
	static function getRegisteredTypes()
	{
		$types = array_keys(SectionContentManagerMap::$map);
		sort($types);
		return $types;
	}
	
	static function onInitialize()
	{
		trace("Registering Section Content Managers", 2);
		ComponentManager::fireEvent('RegisterSectionContentManagers');
	}
}


/**
 * SectionManager provides simple programmatic interface for managing registration of pages to sections.
 * @author andy
 *
 */
class SectionManager
{
	static function addToSection($section_folder, $identifier, $role = "", $template = "", $use_SSL = false, $permissions = "")
	{
		trace("Adding '$identifier' to section '$section_folder'", 3);

		if (SectionManager::isInSection($section_folder, $identifier)) return;
		$section = Section::createFromIdentifier($section_folder);
		
		$content = new SectionContent();
		$content->section_id = $section->section_id;
		$content->identifier = $identifier;
		$content->role = $role;
		$content->permissions = $permissions;
		$content->template = $template;
		$conent->use_SSL = $use_SSL;
		$content->save();
	}
	
	static function removeFromSection($section_folder, $identifier)
	{
		trace("Removing '$identifier' from section '$section_folder'", 3);
		
		$section = Section::createFromIdentifier($section_folder);
		
		$content = new SectionContent();
		$content->delete("WHERE section_id={$section->section_id} AND identifier='$identifier'");
	}
	
	static function isInSection($section_folder, $identifier)
	{
		$section = Section::createFromIdentifier($section_folder);

		$content = new SectionContent();
		return $content->exists("WHERE section_id={$section->section_id} AND identifier='$identifier'");
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new SectionUpgradeManager();
		$mgr->upgrade($version);
	}	

	static function registerSectionContentManager()
	{
		SectionContentManagerMap::registerManager("Content", new SectionContentManager());
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("section", "Sections and Section Content", new SectionSerializationHandler());
		return true;
	}
}

SectionManager::registerSectionContentManager();
?>
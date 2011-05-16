<?php

/**
 * SectionManager provides simple programmatic interface for managing registration of pages to sections.
 * @author andy
 *
 */
class SectionManager
{
	static function addToSection($section_folder, $identifier, $role = "", $template = "", $use_SSL = false)
	{
		trace("Adding '$identifier' to section '$section_folder'", 3);

		if (SectionManager::isInSection($section_folder, $identifier)) return;
		$section = Section::createFromIdentifier($section_folder);
		
		$content = new SectionContent();
		$content->section_id = $section->section_id;
		$content->identifier = $identifier;
		$content->role = $role;
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
}

?>
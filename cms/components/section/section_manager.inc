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

class SectionSerializationHandler implements SerializationHandler
{
	function SectionSerializationHandler()
	{
	}
	
	function export()
	{
		$sections = Query::create(Section, "ORDER BY section_id")->execute();
		$content  = Query::create(SectionContent, "ORDER BY section_content_id")->execute();

		$xml  = "\n<SectionMap>";
		$xml .= toXML("SectionList", $sections, "");
		$xml .= toXML("SectionContentList", $content, "");
		$xml .= "</SectionMap>";
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		trace("== Importing Section Map", 3);
		
		$sectionNodes = $doc->getElementsByTagName("Section");
	
		$sections = array();
		
		foreach($sectionNodes as $node)
		{
			$section = new Section();
			$section->fromXML($node);
			$sections[] = $section;
		}
		
		$contentNodes = $doc->getElementsByTagName("SectionContent");
		
		$sectionContent = array();
		
		foreach($contentNodes as $node)
		{
			$content = new SectionContent();
			$content->fromXML($node);
			$sectionContent[] = $content;
		}
		
		$s = new Section();
		$s->joinTransaction();
		$s->deleteAll();
		$s->disablePrimaryKey();
		
		foreach($sections as $section)
		{
			$section->_disablePK = true;
			$section->joinTransaction($tx);
			$section->save();
		}
			
		$s->enablePrimaryKey();
		
		$c = new SectionContent();
		$c->joinTransaction($tx);
		$c->deleteAll();
		$c->disablePrimaryKey();

		foreach($sectionContent as $content)
		{
			$content->_disablePK = true;
			$content->joinTransaction($tx);
			$content->save();
		}
		
		$c->enablePrimaryKey();
	}	
}

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
	
	static function upgradeComponent()
	{
		$mgr = new SectionUpgradeManager();
		$mgr->upgrade();
	}	

	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("section", "Sections and Section Content", new SectionSerializationHandler());
		return true;
	}
}

?>
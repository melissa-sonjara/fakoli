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
	
	var $fields = array("section_id"		=>	Number,
						"section"			=>	String,
						"section_title"		=>	String,
						"default_template"	=>	String,
						"default_role"		=>	String,
						"default_page"		=>	String,
						"site_id"			=>	Number,
						"use_SSL"			=>	Boolean);
	
	var $relations = array("Content"	=>	SectionContent,
						   "Site"		=>	Site);
	
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
		$content = $this->getContent($identifier);
		if ($content->template) return $content->template;
		return $this->default_template;
	}
	
	function getRole($identifier)
	{
		$content = $this->getContent($identifier);
		if ($content->role) return $content->role;
		return $this->default_role;
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
						"template"				=>	String,
						"use_SSL"				=>	Boolean);
	
	function SectionContent()
	{
		$this->primary_key = "section_content_id";
		$this->table=  "section_content";
		
		$this->DataItem(func_get_args());
	}
}

?>
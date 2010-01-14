<?
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

/**
 * This class maps the publication table.
 */
class Publication extends DataItem
{
	// Fields
	
	var $fields = array("publication_id"		=>	Number,
						"title"					=>	String,
						"publication_year"		=>	Number,
						"journal_id"			=>	Number,
						"pubmed_link"			=>	String,
						"pdf_file"				=>  String,
						"short_abstract"		=>	HTML,
						"abstract"				=>	HTML,
						"keywords"				=>	Text,
						"inclusion_criteria"	=>	Text,
						"intervention"			=>	Text,
						"outcomes"				=>	Text,
						"effect"				=>	Text,
						"funding"				=>	String,
						"public"				=>	Boolean,
						"show_on_home_page"		=>	Boolean,
						"last_modified"			=>	Timestamp,
						"code"					=>	Number);
						
	// Relations
	
	var $relations = array(	"Journal"	=>	Journal,
							"Topics"	=>	Topic,
							"Authors"	=>	Author,
							"Regions"	=>	Region,
							"Samples"	=>	Sample,
							"Settings"	=>	Setting,
							"Foci"		=>	Focus);
							
	function Journal()
	{
		return $this->getRelated(Journal);
	}
	
	function Topics($constraint = "")
	{
		return $this->crossReference(Topic, PublicationTopicXref, $constraint);
	}
	
	function Authors($constraint = "")
	{
		return $this->crossReference(Author, PublicationAuthorXref, $constraint);
	}
	
	function Regions($constraint = "")
	{
		return $this->crossReference(Region, PublicationRegionXref, $constraint);
	}
	
	function Samples($constraint = "")
	{
		return $this->crossReference(Sample, PublicationSampleXref, $constraint);
	}
	
	function Settings($constraint = "")
	{
		return $this->crossReference(Setting, PublicationSettingXref, $constraint);
	}
	
	function Foci($constraint = "")
	{
		return $this->crossReference(Focus, PublicationFocusXref, $constraint);
	}
	
	function Publication()
	{
		$this->primary_key = "publication_id";
		$this->table = "publication";
		
		$this->default_format = "{title}";
		
		$this->DataItem(func_get_args());
	}
}

?>
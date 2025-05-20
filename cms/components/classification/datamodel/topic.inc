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

/**
 * This class maps the topic table.
 */
class Topic extends DataItem
{
	// Fields
	
	var $fields = array("topic_id"	=>	'Number',
						"topic"		=>	'String'
						);
						
	// Relations
	
	var $relations = array(	"Documents"	=>	'Document');
							
	function Documents($constraints = "")
	{
		return $this->crossReference('Document', 'DocumentTopicXref', $constraints);
	}
	
	static function getTopicsArray()
	{
		$topics = query(Topic, "ORDER BY topic");
		
		$tarr = array();
		
		foreach($topics as $topic)
		{
			$tarr[$topic->topic] = $topic->topic;
		}
		
		return $tarr;
	}
		
	function __construct()
	{
		$this->primary_key = "topic_id";
		$this->table = "topic";
		
		$this->default_format = "{topic}";
		
		parent::__construct(func_get_args());
	}
}

class DocumentTopicXref extends DataItem
{
	// Fields
	var $fields = array("document_topic_xref_id"	=>	'Number',
						"document_id"				=>	'Number',
						"topic_id"					=>	'Number');
						
	// Relations
	
	var $relations = array(	"Document"		=>	'Document',
							"Topic"			=>	'Topic');
							
	function Document()
	{
		return $this->getRelated('Document');
	}
	
	function Topic()
	{
		return $this->getRelated('Topic');
	}
	
	function __construct()
	{
		$this->primary_key = "document_topic_xref_id";
		$this->table = "document_topic_xref";
		
		parent::__construct(func_get_args());
	}
}

class GroupTopicXref extends DataItem
{
	// Fields
	var $fields = array("group_topic_id"	=>	'Number',
						"group_id"			=>	'Number',
						"topic_id"			=>	'Number');
						
	// Relations
	
	var $relations = array(	"Group"	=>	'Group',
							"Topic"	=>	'Topic');
							
	function Grp()
	{
		return $this->getRelated('Group');
	}
	
	function Topic()
	{
		return $this->getRelated('Topic');
	}
	
	function __construct()
	{
		$this->primary_key = "group_topic_id";
		$this->table = "grp_topic_xref";
		
		parent::__construct(func_get_args());
	}
}
?>

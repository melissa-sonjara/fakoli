<?php
/**\addtogroup document
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
 * This class maps the publication table.
 */
class Document extends DataItem implements Searchable
{
	// Fields
	
	var $fields = array("document_id"			=>	'Number',
						"title"					=>	'String',
						"file"					=>	'String',
						"url"					=>	'String',
						"publication_date"		=>	'Date',
						"keywords"				=>  'String',
						"author"				=>	'String',
						"publication"			=>	'String',
						"image_id"				=>	'Image',
						"notes"					=>	'HTML',
						"public"				=>	'Boolean',
						"last_modified"			=>	'Timestamp',
						"document_library_id"	=>	'Number',
						"owner_id"				=>	'Number',
						"allow_access"			=>	'String'
	);
						
	// Relations
	
	var $relations = array(	"Topics"			=>	'Topic',
							"Owner"				=>	"",
							"DocumentLibrary"	=>	'DocumentLibrary',
							"Comments"			=>	'Comment',
							"Image"				=>	'ImageRecord'
	);
	
	var $fieldAliases = array("image_id"	=>	"Image", 
							  "url"			=>	"External URL");
	
	function Topics($constraint = "")
	{
		return $this->crossReference('Topic', 'DocumentTopicXref', $constraint);
	}

	function Comments($constraint = "")
	{
		return $this->crossReference('Comment', 'DocumentCommentXref', $constraint);
	}

	function Image()
	{
		return $this->getRelated('ImageRecord');
	}
	
	function Owner()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->owner_id);
	}
	
	function isOwner()
	{
		global $user;
		$owner = $this->Owner();

		return ($owner->user_id == $user->user_id) ? true : false;
	}
	
	/*
	 * For fileshare space, role-based permissions must be anything
	 * in addition or other than just "allow_access" to get edit
	 * details rights to a document.
	 */
	function fileshareAllowEdit()
	{
		global $user;
		
		$library = $this->DocumentLibrary();
		
		$member = $library->getFileshareUser($user->user_id);
		
		if($this->isOwner() || checkRole($library->manage_folders) || 
		checkRole($library->upload_files) || checkRole($library->delete_files) || $member)
			return true;
		else
			return false;
	}
	
	function fileshareAllowDelete()
	{
		$library = $this->DocumentLibrary();
		
		if($this->isOwner() || $library->isOwner() || 
			checkRole($library->delete_files))
			return true;
		else
			return false;
	}

	function DocumentLibrary()
	{
		return $this->getRelated('DocumentLibrary');
	}
	
	function __construct()
	{
		$this->primary_key = "document_id";
		$this->table = "document";
		
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 
		
		$this->default_format = "{title}";
		
		parent::__construct(func_get_args());
	}
	
	function search($params, $range = null)
	{
		trace("Searching Documents", 3);
		

		if ($range)
		{
			$range = " AND {$this->primary_key} IN (".implode($range, ", ").")";
		}
		
		if (is_object($params))
		{
			$search = clone $params;
			$search->target = $this;
			
			if (!$range)
			{
				if (!$search->get("text", "like"))
				{
					$search->secondaryFields("author", "notes", "publication");
				}
				else
				{
					$search->remapField("text", "notes");
				}
			}
			
			$constraint = $search->generateConstraint();
			$constraint .= $constraint ? " AND public=1" : " WHERE public=1";
			
			$constraint .= $range;
			
			$documents = Query::create('Document', $constraint)
							  ->execute();
		}
		else
		{
			$params = preg_replace('/[^\w]+/', ' ', $params);
			$params = "%$params%";
			$documents = Query::create('Document', "WHERE (title like :a OR keywords LIKE :b OR notes Like :c) and public=1 $range")
						  ->bind(":a", $params, ":b", $params, ":c", $params)
						  ->execute();
		}
							  
		return SearchManager::wrap($documents, DocumentSearchResult);
	}
}

class DocumentSearchResult extends AbstractSearchResult
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
		return 1;
	}
	
	function title()
	{
		return $this->item->title;
	}
	
	function date()
	{
		return $this->item->publication_date;
	}
	
	function summary()
	{
		$file = $this->item->file;
			
		$library = $this->item->DocumentLibrary();
		$size = $library->getFileSize($this->item);

		if (Settings::getValue("document", "link_to_details"))
		{
			$link = "/document_details";	
			$linkText = "View Details";
		}
		else
		{
			$link = "/action/document/download";
			$linkText = "Download";
		}
		
		$img = $this->formatIcon(getDocIcon($file), getDocType($file));
		return $this->item->format("<h4>{title}</h4><p><em>{author} {publication_date:short}</em></p>{$img} <a href='{$link}?document_id={document_id}'>{$linkText}</a> $size");
	}
}



class DocumentCommentXref extends DataItem
{
	var $table = "document_comment_xref";
	var $primary_key = "document_comment_id";
	
	var $fields = array("document_comment_id"	=>	'Number',
						"document_id"				=>	'Number',
						"comment_id"				=>	'Number');

	var $relations = array(	"Document"	=>	'Document',
							"Comment"	=>	'Comment');

	function verifyEnabled($document_id)
	{
		if (!$document_id) return false;
		checkNumeric($document_id);
		
		try
		{
			$document = new Document($document_id);
			$library = $document->DocumentLibrary();
			return $library->allow_comments;
		}
		catch(DataNotFoundException $e)
		{
			return false;
		}
	}

	function getURL()
	{
		if(!$this->document_id) return "";

		$url = "document_details?document_id={$this->document_id}";
		return $url . $this->format("#comment{comment_id}");
	}

	function Document()
	{
		return $this->getRelated('Document');
	}

	function Comment()
	{
		return $this->getRelated('Comment');
	}

}


class DocumentSearchAdapter // Implements ISolrAdapter
{

	function getClass()
	{
		return Document;
	}

	function getFilter()
	{
		return "WHERE public=1";
	}

	function getTitleFormat()
	{
		return "{title:xml}";
	}

	function getContentFormat()
	{
		return "";
	}

	function getAuthorFormat()
	{
		return "{author:xml}";
	}

	function getKeywordFormat()
	{
		return "{keywords:xml}";
	}

	function wrap($item)
	{
		return new DocumentSearchResult($item);
	}
}

/**@}*/?>

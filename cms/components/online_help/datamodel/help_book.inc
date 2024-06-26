<?php
/**\addtogroup online_help
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

class HelpBook extends DataItem
{
	// Fields
	var $fields = array("help_book_id"			=>	Number,
						"title"					=>	String,
						"identifier"			=>	String,
						"description"			=>	HTML,
						"searchable"			=>	Boolean,
						"remote"				=>	Boolean,
						"url"					=>	String,
						"additional_css_files"	=>	String);
	
	var $relations = array("Pages"		=>	HelpPage,
			);
	
	
	function Pages($constraint = "ORDER BY sort_order")
	{
		return $this->getRelatedList(HelpPage, "", $constraint);	
	}
	
	static function findByIdentifier($identifier)
	{
		return Query::create(HelpBook, "WHERE identifier=:id")
					->bind(":id", $identifier)
					->executeSingle();
	}
	
	function findPage($identifier)
	{
		$pages = Query::create(HelpPage, "WHERE help_book_id=:book_id AND identifier=:id")
					  ->bind(":book_id", $this->help_book_id, ":id", $identifier)
					  ->execute();
					  
		if (count($pages) > 0) return $pages[0];
		
		return null;
	}
	
	function HelpBook()
	{
		$this->table = "help_book";
		$this->primary_key = "help_book_id";
		
		$this->DataItem(func_get_args());
	}
}
	
/**@}*/?>
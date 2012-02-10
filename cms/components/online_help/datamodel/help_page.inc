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

class HelpPage extends DataItem
{
	// Fields
	var $fields = array("help_page_id"	=>	Number,
						"help_book_id"	=>	Number,
						"parent_id"		=>	Number,
						"title"			=>	String,
						"identifier"	=>	String,
						"content"		=>	HTML,
						"sort_order"	=>	Number,
						"public"		=>	Boolean);
	
	var $relations = array("HelpBook"	=> 	HelpBook,
						   "Parent"		=>	HelpPage,
						   "Children"	=>	HelpPage);
	
	
	function HelpBook()
	{
		return $this->getRelated(HelpBook);
	}
	
	function Parent()
	{
		if (!$this->parent_id) return null;
		
		return $this->getRelated(HelpPage, "parent_id");
	}
	
	function Children($constraint = "ORDER BY sort_order, title")
	{
		return Query::create(HelpPage, "WHERE parent_id=:id $constraint")
					->bind(":id", $this->help_page_id)
					->execute();
	}
		
	function HelpPage()
	{
		$this->table = "help_page";
		$this->primary_key = "help_page_id";
		
		$this->DataItem(func_get_args());
	}
}
	
?>
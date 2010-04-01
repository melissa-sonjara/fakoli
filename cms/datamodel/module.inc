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
 
require_once realpath(dirname(__FILE__)."/../../framework/data_item.inc");

$mod_type = array
(
	NewsItem  =>	 "News Item",
	Program 	=>	 "Program",
	HTML => 		 "HTML" 
);
	
$ord_by = array
(
	created_date	 =>	"created_date", 
	name			 =>	"name",
	last_modified => "last_modified"
	);	
	
/**
 * Defines the Module class.
 */
class Module extends DataItem
{
	var $fields = array("module_id"			=> Number,
						"title"				=> String,
						"group_id"			=> Number,
						"content_type"		=> String,
						"num_items"			=> Number,
						"query_constraint"  => String,
						"ord_by"    		=> String,
						"template"			=> Text,
						"php_code_file"		=> String,
						"menu_id"			=> Number,
						"menu_parameters"	=> String,
						"css_class"			=> String,
						"global"			=> Boolean,
						"global_position"	=> String);
						
	var $relations = array("Menu"	=>	Menu);
	
	function Module()
	{
		$this->table = "module";
		$this->primary_key = "module_id";
		$this->default_format = "{title}";
		$this->DataItem(func_get_args());
	}
	
	function Menu()
	{
		return $this->getRelated(Menu);
	}
	
	function getTitle()
	{
		if ($this->title) return $this->title;
		
	}
	
		function getPdf_file()
	{
		if ($this->pdf_file) return $this->pdf_file;
		
	}
	
	function getContent_Type()
	{
		if ($this->content_type) return $this->content_type;
		
	}
	
	function getConstraint()
	{
		if($this->query_constraint) return $this->query_constraint;
			
	}
	
	function getNum_Items()
	{
		if ($this->num_items) return $this->num_items;
		
	}
	
	function getOrd_By()
	{
		if ($this->ord_by) return $this->ord_by;
		
	}
	
   function getTemplate()
	{
		if ($this->template) return $this->template;
		
	}
	
	function getAdminForm()
	{
		switch($this->content_type)
		{
		case "Menu":
			return "menu_module_form.php";
							
		case "Code":
			return "code_module_form.php";
			
		case "HTML":
			return "html_module_form.php";
			
		default:
			return "query_module_form.php";				
		}
	}
}

?>

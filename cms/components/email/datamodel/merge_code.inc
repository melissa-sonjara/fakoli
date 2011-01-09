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

class MergeCode extends DataItem
{
	var $fields = array(
			"merge_code_id"		=>	Number,
			"name"				=>	String,
			"description"		=>	Text,
			"map"				=>	String,
			"function"			=>	Boolean,
			"class_name"		=>	String
	);

	
	function MergeCode()
	{
		$this->table = "merge_code";
		$this->primary_key = "merge_code_id";
		
		$this->DataItem(func_get_args());		
	}
}


/*
 * Dummy data model (no table) to draw data list view.
 */
class MergeCodeHelp extends DataItem
{
	var $fields = array(
			"help_id"	=>	Number,
			"usage"		=>	String,
			"example"	=>	String
	);
	
	function MergeCodeHelp()
	{
		$this->table = "merge_code";
		$this->primary_key = "help_id";	

	}
	
	static function getHelp()
	{
		$helps = array();
		$help = new MergeCodeHelp();
		$help->usage = "To get a field in the sending class object, enter just the field name as the 'Map' or enter the sending class name dot separator and field name";
		$help->example = "my_field or MySendingClass.my_field";
		$helps[0] = $help;
		
	
		$help = new MergeCodeHelp();
		$help->usage = "To get a field in a related class object, enter the related class name as defined in the DataModel. Use the dot separator to link to the field name. The map does not allow links to relations of relations.";
		$help->example = "MyRelation.my_field";
		$helps[1] = $help;

		$help = new MergeCodeHelp();
		$help->usage = "To get a value from a function in the DataModel of the sending class, enter the function name or the sending class name with dot separator and function name.  You must check the box to indicate the name is a function.";
		$help->example = "'myFunction' or 'MySendingClass.myFunction' and check 'Function'";
		$helps[2] = $help;

		$help = new MergeCodeHelp();
		$help->usage = "To get a value from a function in a related class, enter the related class name as defined in the DataModel with a dot separator and function name. The map does not allow links to relations of relations.";
		$help->example = "'MyRelation.myFunction' and check 'Function'";
		$helps[3] = $help;

		$help = new MergeCodeHelp();
		$help->usage = "Get a value from a callback function.";
		$help->help = "The map must state the name of the function. You must check the box to indicate the name is a function.";
		$help->example = "'myCallbackFunction' and check 'Function'";
		$helps[4] = $help;
		return $helps;
	}
}

?>
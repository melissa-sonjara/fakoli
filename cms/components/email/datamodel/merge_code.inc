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
			"class_name"		=>	String,
			"merge_code_id"		=>	Number,
			"name"				=>	String,
			"description"		=>	Text,
			"map"				=>	String,
			"function"			=>	Boolean // deprecated
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
		$help->usage = "To get a <b>field</b> in the sending class object, a merge code record is not needed; use '[field_name]' as the merge code";
		$help->example = "N/A";
		$helps[0] = $help;
	
		$help = new MergeCodeHelp();
		$help->usage = "To get a <b>field</b> in a <b>related</b> class object, enter the related class name as defined in the DataModel. Use the dot separator to link to the field name. The map does not allow links to relations of relations.";
		$help->example = "MyRelation.my_field";
		$helps[1] = $help;

		$help = new MergeCodeHelp();
		$help->usage = "To get a value from a <b>function</b> in the DataModel of the sending class, enter the function name.";
		$help->example = "myFunction";
		$helps[2] = $help;

		$help = new MergeCodeHelp();
		$help->usage = "To get a value from a <b>function</b> in a <b>related</b> class, enter the related class name as defined in the DataModel with a dot separator and function name. The map does not allow links to relations of relations.";
		$help->example = "MyRelation.myFunction";
		$helps[3] = $help;

		$help = new MergeCodeHelp();
		$help->usage = "To get a value from a <b>callback</b> function, the map must state the name of the function. The function must be available from the calling script. The callback function will receive the sending object as a parameter.";
		$help->example = "myCallbackFunction";
		$helps[4] = $help;
		
		$help = new MergeCodeHelp();
		$help->usage = "To get a value from a static <b>function</b> of a <b>non-DataItem</b> class, enter the class name, dot separator, and static function name. The class definition must be included in the calling script. The static function will receive the sending object as a parameter.";
		$help->example = "myClassName.myStaticFunction";
		$helps[5] = $help;
		
		return $helps;
	}
}


class RelationHelp extends DataItem
{
	var $fields = array(
			"relation_id"	=>	Number,
			"name"			=>	String,
			"class_name"	=>	String
	);
	
	function RelationHelp()
	{
		$this->table = "relation";
		$this->primary_key = "relation_id";	
	}
	
}
?>
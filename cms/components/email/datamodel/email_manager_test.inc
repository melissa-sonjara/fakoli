<?php
/**\addtogroup email
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

/*
 * Datamodel to test email manager
 */
class EmailManagerTest extends DataItem
{
	var $fields = array(
				"test_id"		=>	Number,
				"string"		=>	String,
				"email"			=>	String,
				"date"			=>	Date,
				"relation_id"	=>	Number
	);
	
	var $relations = array(
				"EmailManagerTestRelation"	=>	EmailManagerTestRelation
	);
	
	function TestFunction()
	{
		return "this is the text returned from the function 'TestFunction' in the EmailManagerTest datamodel.";
	}
	
	function EmailManagerTestRelation()
	{
		return $this->getRelated(EmailManagerTestRelation);
	}
	
	function EmailManagerTest()
	{
		$this->table = "email_manager_test";
		$this->primary_key = "test_id";
		
		$this->DataItem(func_get_args());
		
	}
}


class EmailManagerTestRelation extends DataItem
{
	var $fields = array(
			"relation_id"	=>	Number,
			"string"		=>	String
	);
	
	function TestRelationFunction()
	{
		return "this is the text returned from the function 'TestRelationFunction' in the EmailManagerTestRelation datamodel.";
	}
	
	function EmailManagerTestRelation()
	{
		$this->table = "email_manager_test_relation";
		$this->primary_key = "relation_id";
		
		$this->DataItem(func_get_args());
		
	}
}

/*
 * test class callback
 */
class EmailManagerTestClass
{
	function EmailManagerTestClass()
	{
		
	}
	
	static function TestCallback($emailMgrTest)
	{
		return "EmailManagerTestClass::TestCallback ". $emailMgrTest->string;
	}
}


function emailManagerCallbackFunction($obj)
{
	return "EmailManager: emailManagerCallbackFunction " . $obj->string;
}
/**@}*/?>
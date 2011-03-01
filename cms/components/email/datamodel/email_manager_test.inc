<?php

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
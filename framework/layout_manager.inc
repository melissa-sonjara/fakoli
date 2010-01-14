<?php
require_once realpath(dirname(__FILE__))."/auto_form.inc";

class AbstractLayoutManager
{
	var $form;
	
	function AbstractLayoutManager($form)
	{
		$this->form = $form;
	}
	
	function prepareLayout()
	{
		
	}
	
	function writeScript()
	{
		return "";	
	}
	
	abstract function drawForm();
}



?>
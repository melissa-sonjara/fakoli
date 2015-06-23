<?php
require_once realpath(dirname(__FILE__))."/../field_renderers.inc";

/**
 * Provides a very simple digital signature implementation. All it can say is that
 * the correct password was entered by the given user at a given point in time.
 * @author andy
 *
 */
class SignatureFieldRenderer extends StringFieldRenderer
{
	var $errorMessage = "Your signature does not match your password. Please try again.";
	function SignatureFieldRenderer(&$parent)
	{
		$this->StringFieldRenderer($parent);
		$this->password = true;
		$this->autocomplete = false;
	}

	function addValidatorsToForm($field, $required = false)
	{		
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		$this->parent->validator->add(new SignatureValidator($field, $label, $this->errorMessage));
		if ($required)
		{
			$this->parent->required($field);
		}
	}
	
	function renderReadOnly($field = "")
	{
		if (!$field) $field = $this->field;
		$value = $this->parent->data->get($field);
		list($signature,$date,$user_id) = explode(":", $value);
		
		if ($user_id)
		{
			try
			{
				$mgr = new UserManager();
				$u = $mgr->getUser($user_id);
				$msg = "Signed by ".$mgr->getUserFullName($u)." on $date";
			}
			catch(Exception $e)
			{
				$msg = "Invalid Signature";
			}
		}
		else
		{
			$msg = "Not signed";
		}
		
		$this->_startField($field);

		echo $msg;
		
		$this->_endField($field);
	}
	
	function preProcess($field = "")
	{
		global $user;
		
		if (!$_POST[$field]) return;
		
		$obj = $this->parent->data;
		$obj->set($field, $user->hashPassword($_POST[$field]).":".today().":".$user->get($user->getPrimaryKey()));
	}
}
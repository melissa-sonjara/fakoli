<?php
/**\addtogroup fieldrenderers
 * @{
 */

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
	
	var $password = true; 				///< Whether to mask the input in the signature field
	var $autocomplete = false; 			///< Whether to allow autocomplete
	var $signatureField = "password";	///< The field to check for a match when signing
	
	function SignatureFieldRenderer(&$parent)
	{
		$this->StringFieldRenderer($parent);
		$this->password = true;
		$this->autocomplete = false;
	}

	function addValidatorsToForm($field, $required = false)
	{		
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		$this->parent->validator->add(new SignatureValidator($field, $label, $this->errorMessage, $this->signatureField));
		if ($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}
	}
	
	/**
	 * Overrides the default field used to match when signing.
	 * @param string $signatureField the field that is to be match (i.e. email, username)
	 * @param string $errorMessage the error message to display when the match fails
	 */
	function setSignatureField($signatureField, $errorMessage)
	{
		$this->signatureField = $signatureField;
		$this->errorMessage = $errorMessage;
	}
	
	function renderField($field = "")
	{
		if ($this->parent->msg)
		{
			//Clear out field if the form is in error, to force user to re-enter
			$this->parent->data->set($field, '');
		}
		
		if ($this->parent->data->get($field))
		{
			$this->renderReadOnly($field);
		}
		else
		{
			parent::renderField($field);
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
		if ($this->signatureField == "password")
		{
			$obj->set($field, $user->hashPassword($_POST[$field]).":".today().":".$user->get($user->getPrimaryKey()));
		}
		else
		{
			$obj->set($field, $_POST[$field]).":".today().":".$user->get($user->getPrimaryKey());
		}
	}
}

/**@}*/
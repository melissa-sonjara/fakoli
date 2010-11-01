<?php
/*
 * Email Template validation
 * 
 * Loop through the list of emails and check if each is a valid email.
 * Stop at the first invalid email. Ignore merge code emails, e.g., 
 * [user_email]
 */

Fakoli::usingFeature("validation");


/**
 * Email recipient list validator
 *
 * Does not validate nonempty condition - use RequiredValidator
 * for that. Checks if each email address entered is valid.
 * 
 * $field = text or string field containing comma delimited list of email
 * addresses
 *
 * $title - field title to appear in message
 */
class EmailListValidator extends AbstractValidator
{
	var $empty = "";

	function EmailListValidator($field, $title)
	{
		$this->empty = $empty;
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		// no client-side check
	}
	
	function validate()
	{
		global $_POST;
		
		if (isset($_POST[$this->field]))
		{
			$items = $_POST[$this->field];
			$emails = explode(",", $items);
			if(count($emails) > 0)
			{
				foreach($emails as $email)
				{
					// ignore merge codes
					if(preg_match('/^\\[+[a-zA-Z0-9_-]+\\]$/', $email))
						continue;
					
					if(!preg_match('/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b/i', $email))
					{
						return "<warning>{$this->title} has an invalid email address '{$email}'.</warning>";
					}
				}
			}
		}
		else
		{
			return "";
		}
	}
}

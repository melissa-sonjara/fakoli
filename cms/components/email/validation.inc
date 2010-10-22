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
*/*
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

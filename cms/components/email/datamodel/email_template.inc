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


class EmailTemplate extends DataItem
{
	var $fields = array(
				"email_template_id"		=>	Number,
				"class_name"			=>	String,
				"name"					=>	String,
				"sender_email"			=>	String,
				"recipients"			=>	Text,
				"subject"				=>	String,
				"message"				=>	HTML
	);
		
	/**
	 * Retrieves the named email templated
	 * @param string $name the identifier for the email template
	 * @return EmailTemplate the matching email template (null if not matched)
	 */
	static function getEmailTemplate($name)
	{
		$emailTemplates = Query::create(EmailTemplate, "WHERE name=:n")
							   ->bind(":n", $name)
							   ->execute();

		return (count($emailTemplates) > 0) ? $emailTemplates[0] : null;
	}
	
	/**
	 * Checks whether the named EmailTemplate exists
	 * @param string $name the identifier for the email template
	 * @return boolean
	 */
	static function templateExists($name)
	{
		return Query::create('EmailTemplate', "WHERE name=:n")
					->bind(":n", $name)
					->count() > 0;
	}
	
	/**
	 * Create a default template for emails that are sent through Fakoli
	 * components (e.g., blog_published)
	 * 
	 * @param string $name
	 * @param string $recipients
	 * @param string $subject
	 * @param string $message
	 * @param string $class_name
	 * @param string $sender_email
	 */
	static function setDefaultTemplate($name, $recipients, $subject, $message, $class_name, $sender_email = "")
	{
		$emailTemplates = Query::create(EmailTemplate, "WHERE name=:name")
			->bind(":name", $name)
			->execute();
		
		if(count($emailTemplates))
		{
			return;
		}
		
		$emailTemplate = new EmailTemplate();
		$emailTemplate->name = $name;
		$emailTemplate->class_name = $class_name;
		$emailTemplate->recipients = $recipients;
		$emailTemplate->subject = $subject;
		$emailTemplate->message = $message;
		$emailTemplate->save();
	}
		
	function __construct()
	{
		$this->table = "email_template";
		$this->primary_key = "email_template_id";
		
		parent::__construct(func_get_args());
	}
}



/**@}*/?>

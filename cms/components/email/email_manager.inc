<?php
/**************************************************************

Copyright (c) 2007,2008 Sonjara, Inc

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
 * Title: email_manager.inc
 *
 * Description: Class for constructing email messages from
 * a template and sending emails.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/13/10
 *
 ***************************************************************/

class EmailHandler
{
	var $mail;
	var $onSendComplete; // optional user callback function after mail is sent

	function EmailHandler($toAddr, $subject, $message = "", $emailFrom = "")
	{
		global $config;

		if(!$emailFrom) $emailFrom =  $config["email_contact"];
			
		// Use the MD5 algorithm to generate a random hash
		$random_hash = md5(date('r', time()));

		$headers = "From: $emailFrom\r\n";
		$headers .= "Return-Path: ". $emailFrom . "\r\n";
		$headers .= "Reply-To: ". $emailFrom . "\r\n";
		$headers .= "Mime-Version: 1.0\r\n";

		if($config["alt_text_email"] == false)
			$headers .= "Content-type: text;\r\n";
		else
			$headers .= "Content-Type: multipart/alternative; boundary=\"--PHP-alt-".$random_hash."\"\r\n";

		if($message AND $config["alt_text_email"])
			$body = $this->getMessageScript($message, $random_hash);
		elseif($message)
		{
			$patterns = array("/<br>/", "/&nbsp;/", "/<p>/", "/<BR>/", "/<P>/");
			$replacements = array("\r\n", " ", "\r\n", "\r\n", "\r\n");
			$message =  preg_replace($patterns, $replacements, $message);
			$body = trim(stripHTML($message));
		}
		
		//trace("EmailHandler:: toAddr: $toAddr; emailFrom: $emailFrom; subject: $subject, message: $body", 3);
		$this->mail = new Mail($emailFrom, $toAddr, $subject, $body, $headers);
	}

	/*
	 * This isn't working on zither; works locally - zither can't determine
	 * which view to send and sends both but shows the html as text with tags
	 */
	function getMessageScript($msg, $random_hash)
	{
		global $config;
		
		if ($config['html_email_template'])
		{
			$file = $config['homedir'] . $config['html_email_template'];
			trace("Loading email template '$file'", 3);
			$template = file_get_contents($file);
			$template = str_replace("{var:styles}", Fakoli::getStyles(), $template);
			$html = str_replace("{message}", $msg, $template);
		}
		else
		{
			$html = $msg;
		}		
		
		//define the body of the message.
		ob_start(); //Turn on output buffering
		?>

----PHP-alt-<?php echo "$random_hash\n"; ?>
Content-Type: text/plain
Content-Disposition: inline
Content-Transfer-Encoding: 8bit

<? echo trim(stripHTMLTags($msg)); ?>


----PHP-alt-<?php echo "$random_hash\n"; ?>
Content-Type: text/html
Content-Disposition: inline
Content-Transfer-Encoding: 8bit

<?echo $html ?>

----PHP-alt-<?php echo $random_hash; ?>--
		<?
		//copy current buffer contents into $message variable
		// and delete current output buffer
		$message = ob_get_clean();

		return $message;

	}

	function send()
	{
		$rtn = $this->mail->send();
		
		// Call the callback whether successful or not; calling script
		// decides how to handle either way. 
		if ($this->onSendComplete AND is_callable($this->onSendComplete))
		{
			call_user_func($this->onSendComplete, $mail, $rtn);
		}			
		return $rtn;
	}
}



class EmailManager
{
	var $emailTemplate;
	var $item; // dataitem object of the base class sending the emails
	var $recipients;
	var $message;
	var $subject;
	var $emailFrom;
	var $onSendComplete; // optional user-defined callback function after send is successful

	function EmailManager($item, $emailTemplate, $onSendComplete = "")
	{
		$this->item = clone($item);
		$this->item->filter = null;
		$this->emailTemplate = $emailTemplate;
		$this->emailFrom = $emailTemplate->sender_email;
		$this->mergeEmail();
		$this->onSendComplete = $onSendComplete;
	}


	/*	
	 * Use MergeCodeManager, which shares functionality
	 * with TextLookup.  
	 */
	function mergeEmail()
	{
		$emailText = $this->emailTemplate->recipients . $this->emailTemplate->subject . $this->emailTemplate->message;
		$mgr = new MergeCodeManager($this->item, $emailText);
	
		$fields = array("recipients", "subject", "message");
		foreach($fields as $field)
			$this->$field = $mgr->searchAndReplace($this->emailTemplate->$field);			
	}	

	function getRecipients()
	{
		$validRecipients = array();
		
		trace("EmailManager:getRecipients {$this->recipients}", 3);
		if($this->recipients)
		{
			$recipients = explode(",", $this->recipients);
			if(count($recipients) > 0)
			{
				foreach($recipients as $recipient)
				{
					if(preg_match('/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b/i', $recipient))
						$validRecipients[] = $recipient;
					else
						trace("EmailManager:getRecipients invalid recipient $recipient", 3);					
				}
			}
		}

		return $validRecipients;
	}
	
	/*
	 * Call the callback whether successful or not; calling script
	 * decides how to handle either way. Each recipient could have
	 * different result so we don't record multiple recipients in
	 * one record.
	 */
	
	function sendEmail()
	{
		$recipients = $this->getRecipients();
		$rtn = false;
		
		if(count($recipients) > 0)
		{
			foreach($recipients as $recipient)
			{
				$rtn = $this->sendOneEmail($recipient);
			}
		}
		else
		{
			trace("EmailManager:: Warning - no valid recipients", 3);
		}
		return $rtn;
	}
	
	function sendOneEmail($recipient)
	{
		trace("EmailManager:: sending email to recipient $recipient", 3);
				
		$email = new EmailHandler($recipient, $this->subject, $this->message, $this->emailFrom);
				
		$rtn = $email->send();
			
		if ($this->onSendComplete AND is_callable($this->onSendComplete))
		{
			call_user_func($this->onSendComplete, $this, $recipient, $rtn);
		}			
		
		return $rtn;
	}
	
	static function setDefaults()
	{
		trace("EmailManager::setDefaults", 3);
		
		global $config;
		
		Settings::setDefaultValue("email", "use_debugging_mode", false, "Boolean");
		
		$defaultPath = $config['uploadbase'] . DIRECTORY_SEPARATOR . "email_debug_logs";
		
		if (!file_exists($defaultPath)) mkdir($defaultPath);
		
		Settings::setDefaultValue("email", "debugging_mode_output_path", $defaultPath, "String");
	}

}


/*
 * $sendindItem - object of a DataModel class. The templates class_name
 * must be the same class as the object
 * 
 * $name - name of the email template to be sent
 * 
 * $onSendComplete - optional user callback function, e.g., record
 * email to log file
 */
function sendEmailUsingEmailManager($sendingItem, $name, $onSendComplete = "")
{
	$emailTemplate = EmailTemplate::getEmailTemplate($name);

	if($emailTemplate)
	{
		trace("Sending email '$name'", 3);
		$emailManager = new EmailManager($sendingItem, $emailTemplate, $onSendComplete);
		$rtn = $emailManager->sendEmail();
		trace("EmailManager:: rtn code $rtn", 3);
	}
	
	return $rtn;
}

?>
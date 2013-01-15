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

Fakoli::using("settings");


class EmailHandler
{
	var $mail;
	var $onSendComplete; // optional user callback function after mail is sent

	/**
	 * 
	 * @param String $toAddr
	 * @param String $subject 
	 * @param String $message
	 * @param String $emailFrom - if blank, use settings email_from; if not blank and != settings, set this value into Reply To header instead
	 * @param String $emailName - same as above
	 */
	function EmailHandler($toAddr, $subject, $message = "", $emailFrom = "", $emailName = "")
	{
		global $config;

		$site_email_from =  Settings::getValue('email', 'email_from');
		$site_email_name =  Settings::getValue('email', 'email_name');
		
		$replyTo = $site_email_from;
		$replyToName = $site_email_name;
		
		if($emailFrom)
		{
			$replyTo = $emailFrom;
		}
		
		if($emailName)
		{
			$replyToName = $emailName;
		}
		
		// Use the MD5 algorithm to generate a random hash
		$random_hash = md5(date('r', time()));
		$headers = "From: ". $site_email_name . " <" . $site_email_from . ">\r\n";
		$headers .= "Return-Path: ". $site_email_name . "\r\n";
		$headers .= "Reply-To: ". $replyTo . " <" . $replyToName . ">\r\n";
		$headers .= "Mime-Version: 1.0\r\n";

		$headers .= "Content-Type: multipart/alternative; boundary=\"--PHP-alt-".$random_hash."\"\r\n";

		$message = $this->getMessageScript($message, $random_hash);
			
		trace("Instantiating Mail: site_email_from $site_email_from toAddr $toAddr headers $headers", 3);
		$this->mail = new Mail($site_email_from, $toAddr, $subject, $message, $headers);
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

<?php echo trim(stripHTMLTags($msg)); ?>


----PHP-alt-<?php echo "$random_hash\n"; ?>
Content-Type: text/html
Content-Disposition: inline
Content-Transfer-Encoding: 8bit

<?php echo $html ?>

----PHP-alt-<?php echo $random_hash; ?>--
		<?php
		//copy current buffer contents into $message variable
		// and delete current output buffer
		$message = ob_get_clean();

		return $message;

	}

	function send()
	{
		$rtn = $this->mail->send();
		
		trace("EmailHandler send rtn $rtn", 3);
		// Call the callback whether successful or not; calling script
		// decides how to handle either way. 
		if ($this->onSendComplete AND is_callable($this->onSendComplete))
		{
			call_user_func($this->onSendComplete, $mail, $rtn);
		}			
		return $rtn;
	}
}


/*
 * Takes an email template and an obj of any DataItem class
 * and sends email to a list of recipients after substituting
 * merge codes for values from the obj.
 * 
 * For example: message text "Dear [first_name]" becomes
 * Dear John Smith
 * 
 * @item: obj of DataItem class that has the field values to be used
 * in the email (e.g., user, program, order).
 * 
 * @emailTemplate: instance of DataItem EmailTemplate. This can
 * be an instance constructed on the fly rather than retrieved from
 * the db. If constructed, the following fields must be set:
 *   recipients
 *   subject
 *   message
 *   class_name
 * 
 * @onSendComplete: optional callback function to record the
 * email that was sent in a log.
 * 
 * The following var are set in function mergeEmail after
 * the MergeCodeManager performs the substitutions:
 * 
 * recipients
 * subject
 * message
 */
class EmailManager
{
	var $emailTemplate;
	var $item; // dataitem object of the base class sending the emails
	var $recipients;
	var $message;
	var $subject;
	var $emailFrom;
	var $emailName;
	var $onSendComplete; // optional user-defined callback function after send is successful

	function EmailManager($item, $emailTemplate, $onSendComplete = "")
	{
		$this->item = clone($item);
		$this->item->filter = null;
		$this->emailTemplate = $emailTemplate;
		$this->emailName = $emailTemplate->name;
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
		$mgr = new MergeCodeManager($this->item, $emailText, $this->emailTemplate->class_name);
	
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
				
		$email = new EmailHandler($recipient, $this->subject, $this->message, $this->emailFrom, $this->emailName);
				
		$rtn = $email->send();
			
		if ($this->onSendComplete AND is_callable($this->onSendComplete))
		{
			call_user_func($this->onSendComplete, $this, $recipient, $rtn);
		}			
		
		return $rtn;
	}
	
	static function send($sendingItem, $name, $onSendComplete = "")
	{
		$emailTemplate = EmailTemplate::getEmailTemplate($name);

		if($emailTemplate)
		{
			trace("Sending email '$name'", 3);
			$emailManager = new EmailManager($sendingItem, $emailTemplate, $onSendComplete);
			$rtn = $emailManager->sendEmail();
			trace("EmailManager:: rtn code $rtn", 3);
		}
		else
		{
			$rtn = 0;
			trace("EmailManager::no template found", 3);
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
		Settings::setDefaultValue("email", "email_from", "andy@sonjara.com", "String", "From address for generated emails");
		Settings::setDefaultValue("email", "email_contact", "andy@sonjara.com", "String", "To address for generated emails");
		Settings::setDefaultValue("email", "email_name", "Andy Green", "String", "Name of email contact");
		Settings::setDefaultValue("email", "mail_server", "timpani.sonjara.com", "String");
		Settings::setDefaultValue("email", "alt_text_email", false, "Boolean", "Whether the mail server can send html-formatted email messages");
		
	}	
	
	static function upgradeComponent($version)
	{
		$mgr = new EmailUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	/*
	 * Scan the email debug log path to retrieve
	 * all the log files.
	 */
	static function scanDebugLogs()
	{
		$path = Settings::getValue("email", "debugging_mode_output_path");
	
		$logs = array();
		
		trace("EmailManager::Scanning $path", 3);

		$handle = opendir($path);
		
		if(!$handle)
			return;
			
		$idx = 0;
		while(false !== ($file = readdir($handle)))
		{
			// omit "." and ".." in the directory
			if (!preg_match("/(^\.{1,2}$)/i", $file))
			{
				$log = new EmailDebugLog();
				$log->log_id = $idx;
				$log->filename = $file;
				$log->date = date("F d, Y g:ia", (filemtime($path . DIRECTORY_SEPARATOR . $file)));
			 	$logs[$log->log_id] = $log;
			 	$idx++;
			}
		}
		    
	    closedir($handle);
		
	    return $logs;
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("email_templates", "Email Templates", new SimpleSerializationHandler(EmailTemplate));
		SerializationManager::registerHandler("merge_codes", "Merge Codes", new SimpleSerializationHandler(MergeCode));
		return true;		
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
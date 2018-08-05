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
	 * @param Array $attachments - optional array of full file paths to attach. Key is display name of file 
	 * 				(which may be the same as the basename of the file path), value is santized file path.
	 * 				If a string is received, it will be conerted to an array with the key the basename of the path
	 * @param iCalendarEventManager obj $iCalMgr
	 */
	function EmailHandler($toAddr, $subject, $message = "", $emailFrom = "", $emailName = "", $attachments = array(), $iCalMgr = null)
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
		
		$toAddr = trim($toAddr);
		
		// Use the MD5 algorithm to generate a random hash
		$random_hash = md5(date('r', time()));
		$from = $site_email_name . " <" . $site_email_from . ">";
		$headers .= "Return-Path: ". $site_email_from . "\r\n";
		$headers .= "Reply-To: ". $replyToName . " <" . $replyTo  . ">\r\n";
		$headers .= "MIME-Version: 1.0\r\n";

		$headers .= "Content-Type: multipart/alternative; boundary=\"--PHP-alt-".$random_hash."\"\r\n";

		if($attachments && !is_array($attachments)) 
		{
			$attachments = array_combine(basename($attachments), $attachments);
		}
		
		if($iCalMgr)
		{
			$iCalMgr->setOrganizer($replyTo, $replyToName);
			$iCalMgr->setAttendee($toAddr);
		}
		$message = $this->getMessageScript($message, $random_hash, $attachments, $iCalMgr);
		
		$this->mail = new Mail($site_email_from, $toAddr, $subject, $message, $headers, $from);
	}

	/*
	 * This isn't working on zither; works locally - zither can't determine
	 * which view to send and sends both but shows the html as text with tags
	 */
	function getMessageScript($msg, $random_hash, $attachments, $iCalMgr)
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

		$fileAttachments = $this->formatAttachments($attachments, $random_hash);
		
		if($iCalMgr)
		{
			$iCal = $this->formatiCalendar($iCalMgr, $random_hash);
		}
		
		//define the body of the message.
		ob_start(); //Turn on output buffering
		?>

----PHP-alt-<?php echo "$random_hash\n"; ?>
Content-Type: text/plain
Content-Disposition: inline
Content-Transfer-Encoding: 8bit

<?php echo trim(HTMLToText($msg)); ?>


----PHP-alt-<?php echo "$random_hash\n"; ?>
Content-Type: text/html
Content-Disposition: inline
Content-Transfer-Encoding: 8bit

<?php echo $html ?>

<?php echo $fileAttachments ?>

<?php echo $iCal ?>

----PHP-alt-<?php echo $random_hash; ?>--
		<?php
		//copy current buffer contents into $message variable
		// and delete current output buffer
		$message = ob_get_clean();

		return $message;
	}
	
	/**
	 * 
	 * @param HTML $message
	 * @param Array $files - files to attach
	 */
	function formatAttachments($attachments, $random_hash)
	{
		if(!count($attachments)) return "";
		
		foreach($attachments as $display_name => $attachment)
		{
			if(is_file($attachment))
			{
				$base_filename = basename($attachment);
				
				$fileAttachments .= "----PHP-alt-{$random_hash}\n";
				$fp =  @fopen($attachment, "rb");
				$fileContents = @fread($fp, filesize($attachment));
				@fclose($fp);
				$fileContents = chunk_split(base64_encode($fileContents));
				$fileAttachments .= "Content-Type: application/octet-stream; name=\"". $base_filename ."\"\n" .
						"Content-Description: ".$base_filename."\n" .
						"Content-Disposition: attachment;\n" . " filename=\"".$display_name ."\"; size=".filesize($attachment).";\n" .
						"Content-Transfer-Encoding: base64\n\n" .$fileContents . "\n\n";
			}
		}
		
		return $fileAttachments;
	}	
	
	/**
	 * The iCal event should come in formatted by the event
	 * handler formatICal function. 
	 * 
	 * Here we provide the organizer's email if not already
	 * set, based on the reply to email and format the header for this block.
	 */
	function formatiCalendar($iCalMgr, $random_hash)
	{	
		$method = $iCalMgr->data->get("method");
		
		$cal_message = "----PHP-alt-{$random_hash}\n";			
		$cal_message .= "Content-Type: text/calendar; charset=utf-8;method={$method}\n";
		$cal_message .= "Content-Disposition: inline; filename=meeting.ics\n";
		$cal_message .= "Content-Transfer-Encoding: 8bit\n";
				
		$cal_message .= $iCalMgr->format(); 
	
		return $cal_message;
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


/**
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
	var $emailName;					///< The name of the email sender (optional)
	var $emailTemplateName;			///< The name of the email template
	var $onSendComplete; // optional user-defined callback function after send is successful
	var $attachments; ///< optional - array of files to attach to the email
	
	function EmailManager($item, $emailTemplate, $onSendComplete = "")
	{
		$this->item = clone($item);
		$this->item->filter = null;
		$this->emailTemplate = $emailTemplate;
		$this->emailTemplateName = ($this->item) ? $this->item->format($emailTemplate->name) : $emailTemplate->name;
		$this->emailFrom = ($this->item) ? $this->item->format($emailTemplate->sender_email) : $emailTemplate->sender_email;
		$this->cleanupTemplate();
		$this->mergeEmail();
		$this->onSendComplete = $onSendComplete;
	}

	/**
	 * Fixes up some common issues introduced by the TinyMCE HTML editor, and provides
	 * a base href tag for the message if a full HTML document is found.
	 */
	private function cleanupTemplate()
	{
		trace("** Cleaning Up Email Template HTML", 3);
		
		$this->emailTemplate->message = str_replace("<p><!DOCTYPE", "<!DOCTYPE", $this->emailTemplate->message);
		$this->emailTemplate->message = str_replace("<p><html", "<html", $this->emailTemplate->message);
		$this->emailTemplate->message = str_replace("</html></p>", "</html>",  $this->emailTemplate->message);
		$baseURL = Settings::getValue("email", "HTML_email_base_url");
		if ($baseURL && stripos($this->emailTemplate->message, "<head>"))
		{
			$this->emailTemplate->message = str_replace("<head>", "<head><base href='{$baseURL}'>", $this->emailTemplate->message);
		}
		
		$this->emailTemplate->message = "XXXXXX";
	}
	
	/**	
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
				
		$email = new EmailHandler($recipient, $this->subject, $this->message, $this->emailFrom, $this->emailName, $this->attachments);
			
		$rtn = $email->send();
			
		if ($this->onSendComplete AND is_callable($this->onSendComplete))
		{
			call_user_func($this->onSendComplete, $this, $recipient, $rtn);
		}			
		
		EmailLog::logEmail($this, $recipient, $rtn);
		return $rtn;
	}
	
	/**
	 * Sends an email template, based on the supplied DataItem (or array of DataItems)
	 * @param mixed $source either a single DataItem, or an array of DataItems
	 * @param string $name the identifier of the email template 
	 * @param callable $onSendComplete callback handler to be called after all generated emails have been sent
	 * @return number|boolean
	 */
	static function send($source, $name, $onSendComplete = "")
	{
		$emailTemplate = EmailTemplate::getEmailTemplate($name);

		if($emailTemplate)
		{
			if (is_array($source))
			{
				$sendingItem = new JoinResult();
				foreach($source as $item)
				{
					$cl = get_class($item);
					$sendingItem->$cl = $item;
				}
			}
			else
			{
				$sendingItem = $source;
			}
			
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
		
		Settings::setDefaultValue("email", "use_debugging_mode", false, "Boolean", "Enable local logging of generated emails (instead of sending via SMTP)", "Debugging Mode", null, 0);
		
		$defaultPath = $config['uploadbase'] . DIRECTORY_SEPARATOR . "email_debug_logs";
		
		if (!file_exists($defaultPath)) mkdir($defaultPath);
		
		Settings::setDefaultValue("email", "debugging_mode_output_path", $defaultPath, "String", "Folder that will store the logged emails in debugging mode", "Debugging Mode", null, 1);
		Settings::setDefaultValue("email", "email_from", "andy@sonjara.com", "String", "Default From address for generated emails", "Contact Details", null, 1);
		Settings::setDefaultValue("email", "email_contact", "andy@sonjara.com", "String", "Default To address for generated emails", "Contact Details", null, 2);
		Settings::setDefaultValue("email", "email_name", "Andy Green", "String", "Default Name of email contact for generated emails", "Contact Details", null, 3);
		Settings::setDefaultValue("email", "default_contact_us_recipient", "", "String", "Default email address(es) that will receive messages from the Contact Us form. These are used if no Contact Topics are defined", "Contact Details", null, 4);
		Settings::setDefaultValue("email", "use_captcha", "Never", String, "Specify when to use a Captcha to verify the user", "Contact Details", "Never\nAnonymous Only\nAlways", 5);
		Settings::setDefaultValue("email", "HTML_email_base_url", "", String, "", "Contact Details", null, 6);
		
		Settings::setDefaultValue("email", "override_PHP_settings", false, Boolean, "Use these server settings instead of the global settings in your php.ini file", "Server Settings", 0);
		Settings::setDefaultValue("email", "mail_server", "localhost", "String", "Host name or IP address of your upstream SMTP server", "Server Settings", "", 1);
		Settings::setDefaultValue("email", "mail_server_port", 25, Number, "Port to use on the upstream SMTP server", "Server Settings", "", 2);
		Settings::setDefaultValue("email", "username", "", String, "SMTP Account username (if authentication is required)", "Server Settings", "", 3);
		Settings::setDefaultValue("email", "password", "", String, "SMTP Account password (if authentication is required)", "Server Settings", "", 4);
		
		Settings::setDefaultValue("email", "log_messages", false, Boolean, "Log messages in the database after sending", "Message Logging", "", 1);
		Settings::setDefaultValue("email", "log_message_bodies", false, Boolean, "Specifies whether to store the message text when logging an email message. ".
																				 "It is recommended to leave this unchecked to conserve space in the database.", "Message Logging", "", 2);
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
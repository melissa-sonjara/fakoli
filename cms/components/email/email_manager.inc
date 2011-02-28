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
			$headers .= "\r\nContent-Type: multipart/alternative; boundary=\"--PHP-alt-".$random_hash."\"";

		if($message AND $config["alt_text_email"])
			$body = $this->getMessageScript($message, $random_hash);
		elseif($message)
		{
			$patterns = array("/<br>/", "/&nbsp;/", "/<p>/", "/<BR>/", "/<P>/");
			$replacements = array("\r\n", " ", "\r\n", "\r\n", "\r\n");
			$message =  preg_replace($patterns, $replacements, $message);
			$body = stripHTML($message);
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

		//define the body of the message.
		ob_start(); //Turn on output buffering
		?>

----PHP-alt-<?php echo $random_hash; ?>
Content-Type: text/plain; charset="iso-8859-1"
Content-Transfer-Encoding: quoted-printable

		<? echo stripHTMLTags($msg); ?>


----PHP-alt-<?php echo $random_hash; ?>
Content-Type: text/html; charset="iso-8859-1"
Content-Transfer-Encoding: quoted-printable

		<?echo $msg ?>

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
		$this->item->filter = "";
		$this->emailTemplate = $emailTemplate;
		$this->emailFrom = $emailTemplate->sender_email;
		$this->mergeEmail();
		$this->onSendComplete = $onSendComplete;
	}


	function mergeEmail()
	{
		$mergeCodes = query(MergeCode, "WHERE class_name = '{$this->emailTemplate->class_name}'");
		
		//echo "<pre>"; print_r($this->emailTemplate); echo "</pre>";
		//echo "<pre>"; print_r($mergeCodes); echo "</pre>";

		if(count($mergeCodes) > 0)
		{
			$patterns = array();
			$replacements = array();
				
			foreach($mergeCodes as $mergeCode)
			{
				if($mergeCode->map)
				{
					$replaceText = $this->getReplaceText($mergeCode->map);
						
					// Even if replace text is empty, still add to
					// 	list to get rid of code from the text (e.g., [merge_code])
					array_push($patterns, "[$mergeCode->name]");
					array_push($replacements, $replaceText);
				}
			}
		}

		//echo "<pre>"; print_r($patterns); echo "</pre>";
		//echo "<pre>"; print_r($replacements); echo "</pre>";
		
		$this->recipients = $this->searchReplaceText($patterns, $replacements, $this->emailTemplate->recipients);
		$this->subject = $this->searchReplaceText($patterns, $replacements, $this->emailTemplate->subject);
		$this->message = $this->searchReplaceText($patterns, $replacements, $this->emailTemplate->message);
	}
	
	
	/*
	 * If the mergeCode map has 2 parts, then possible interpretations: 
	 * a) Part 1 is relation and part 2 is field of relation
	 * b) part 1 is relation and part 2 is function in that relation
	 * c) part 1 is a class and part2 is a static function in that class
	 *
	 * 
	 * If the mergeCode map has 1 part, then it must be
	 * a) callback function, no class
	 * b) function of calling class
	 * c) field of calling class
	 */
	function getReplaceText($mergeCodeMap)
	{	
		$valid = true;
					
		list($part1, $part2) = explode(".", $mergeCodeMap);

		trace("EmailManager::part1 is $part1 and part2 is $part2", 3);
			
		if(get_class($this->item) == $part1)
		{
			trace("EmailManager::part1 equals item class", 3);
			$part1 = $part2;
			$part2 = "";
		}			
				
		if($part2)
		{
			if($this->item->hasRelation($part1))
			{
				$replaceObj = $this->item->$part1();
				if(is_callable(array($replaceObj, $part2)))
				{
					trace("EmailManager:: part1 is relation of sending class and part2 is a function in that relation.", 3);
					$replaceText = $replaceObj->$part2();
				}	
				elseif(is_object($replaceObj) && $replaceObj->hasField($part2))
				{
					trace("EmailManager:: part1 is relation and part2 is a field in that related class.", 3);
					$replaceText = $replaceObj->format("{".$part2 ."}");
				}
				else
				{
					$valid = false;
				}
			}
			elseif(is_callable(array($part1, $part2)))
			{
				trace("EmailManager:: part1 is class and part2 is static function of a class.", 3);
				$replaceText = call_user_func(array($part1, $part2), $this->item);
			}
			else
			{
				$valid = false;
			}
		}
		elseif($part1) // Map has just one part
		{
			if(is_callable($part1))
			{
				trace("EmailManager:: part1 is callback function.", 3);
				$replaceText = call_user_func($part1, $this->item);
			}
			elseif(is_callable(array($this->item, $part1)))
			{
				trace("EmailManager:: part1 is function from sending class.", 3);
				$replaceText = $this->item->$part1();
			}
			elseif($this->item->hasField($part1))
			{
				trace("EmailManager:: part1 is field of sending class.", 3);
				$replaceText = $this->item->format("{".$part1 ."}");
			}
			else
			{
				$valid = false;
			}
		}
		
		if($valid)
			trace("EmailManager:: replaceText is $replaceText", 3);
		else
			trace("EmailManager:: Warning: merge code map not valid.", 3);
	
		return $replaceText;		
	}
	
	function searchReplaceText($patterns, $replacements, $text)
	{
		$lText = "";
		
		if(count($patterns) > 0 AND $text != "")
		{
			$lText = str_replace($patterns, $replacements, $text);
		}
		else
			$lText = $text;
		
		return $lText;
	}
	
	function getRecipients()
	{
		$validRecipients = array();
		
		trace("EmailManager:getRecipients", 3);
		if($this->recipients)
		{
			$recipients = explode(",", $this->recipients);
			if(count($recipients) > 0)
			{
				foreach($recipients as $recipient)
				{
					if(preg_match('/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b/i', $recipient))
						array_push($validRecipients, $recipient);
				}
			}
		}
		trace("EmailManager:getRecipients recipient count". count($validRecipients), 3);
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
		trace("EmailManager:: sendEmail", 3);
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
			trace("EmailManager:: Warning - no recipients", 3);
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
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
			$headers .= "\r\nContent-Type: multipart/alternative; boundary=\"PHP-alt-".$random_hash."\"";

		if($message AND $config["alt_text_email"])
			$body = $this->getMessageScript($message, $random_hash);
		elseif($message)
		{
			$patterns = array("/<br>/", "/&nbsp;/", "<br/>");
			$replacements = array("\r\n", " ", "\r\n");
			$message =  preg_replace($patterns, $replacements, $message);
			$body = stripHTML($message);
		}
		
		trace("EmailHandler:: toAddr: $toAddr; emailFrom: $emailFrom; subject: $subject, message: $body", 3);
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

--PHP-alt-
		<?php echo $random_hash; ?>
Content-Type: text/plain; charset="iso-8859-1"
Content-Transfer-Encoding: 7bit

		<? echo stripHTMLTags($msg); ?>


--PHP-alt-
		<?php echo $random_hash; ?>
Content-Type: text/html; charset="iso-8859-1" Content-Transfer-Encoding:
7bit

		<?echo $msg ?>

--PHP-alt-
		<?php echo $random_hash; ?>
--
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
		if ($this->onSendComplete)
		{
			$onComplete = $this->onSendComplete;
			$onComplete($this->mail, $rtn);
		}
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
	var $preview;
	var $onSendComplete; // optional user-defined callback function after send is successful

	function EmailManager($item, $emailTemplate, $preview = false)
	{
		$this->item = $item;
		$this->emailTemplate = $emailTemplate;
		$this->emailFrom = $emailTemplate->sender_email;
		$this->mergeEmail();
		$this->preview = $preview;
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
				
				$replaceText = $this->getReplaceText($mergeCode);
						
				// Even if replace text is empty, still add to
				// list to get rid of code from the text (e.g., [merge_code])
				array_push($patterns, "[$mergeCode->name]");
				array_push($replacements, $replaceText);
			}
		}

		//echo "<pre>"; print_r($patterns); echo "</pre>";
		//echo "<pre>"; print_r($replacements); echo "</pre>";
		
		$this->recipients = $this->searchReplaceText($patterns, $replacements, $this->emailTemplate->recipients);
		$this->subject = $this->searchReplaceText($patterns, $replacements, $this->emailTemplate->subject);
		$this->message = $this->searchReplaceText($patterns, $replacements, $this->emailTemplate->message);
	}
	
	
	function getReplaceText($mergeCode)
	{
		if(is_callable($mergeCode->map))
		{
			$replaceText = call_user_func($mergeCode->map, $this->item);
		}
		else
		{
			list($relation, $field) = explode(".", $mergeCode->map);
			
			trace("EmailManager::relation is $relation and field is $field", 3);
			$className = get_class($this->item);

			// if the value_obj is the same as calling class then just keep that
			if($relation AND $field)
			{
				if(get_class($this->item) == $relation)
				{
					$replaceObj = $this->item;
				}
				elseif($this->item->hasRelation($relation))
				{
					$replaceObj = $this->item->$relation();
				}
				else
				{
					trace("EmailManager::item does not have the specified relation", 3);
				}
			}
			else // we have a function from the calling class e.g., getUrl()
				$replaceObj = $relation;
			
			// such as Order.formatOrder(), a function in the Order datamodel, called
			// from sending class Program
			if(is_object($replaceObj) AND $mergeCode->function)
			{
				$replaceText = $replaceObj->$field();
				trace("EmailManager::replaceObj is related class and field is function; " . get_class($replaceObj). " $field; replaceText: $replaceText", 3);
			}
			
			// e.g., Program->name, a field in Program datamodel
			elseif(is_object($replaceObj))
			{
				$replaceText = $replaceObj->format("{".$field ."}");
				trace("EmailManager::replaceObj is object; text is $replaceText", 3);
			}
			
			// e.g., Program.orderList, a function called from the sending
			// class Program
			else if ($replaceObj AND $mergeCode->function) // function called from the sending class
			{
				$replaceText = $this->item->$replaceObj();	
				trace("EmailManager::replaceText is function from sending class", 3);
			}
			elseif($replaceObj) // field of the calling class e.g., "comment"
			{
				return $replaceText = $this->item->$replaceObj;
				trace("EmailManager::replaceText is field of the calling class", 3);
			}
		}
		
		trace("replaceText is $replaceText", 3);
		return $replaceText;		
	}
	
	function searchReplaceText($patterns, $replacements, $text)
	{
		trace("EmailManager::searchReplaceText start text $text", 3);
		$lText = "";
		
		if(count($patterns) > 0 AND $text != "")
		{
			$lText = str_replace($patterns, $replacements, $text);
		}
		else
			$lText = $text;
		
		trace("EmailManager::searchReplaceText end text $lText", 3);
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

	function sendEmail()
	{
		trace("EmailManager:: sendEmail", 3);
		$recipients = $this->getRecipients();
		
		if(count($recipients) == 0)
		{
			trace("EmailManager:: Warning - no recipients", 3);
			return;
		}
			
		foreach($recipients as $recipient)
		{
			trace("EmailManager:: sending email to recipient $recipient", 3);
			
			$email = new EmailHandler($recipient, $this->subject, $this->message, $this->emailFrom);
			
			$rtn = $email->send();
			
			// Call the callback whether successful or not; calling script
			// decides how to handle either way. 
			if ($this->onSendComplete)
			{
				$onComplete = $this->onSendComplete;
				$onComplete($this, $email, $rtn);
			}
			
		}
		return $rtn;
	}
}



function sendEmailUsingEmailManager($sendingItem, $name)
{
	$emailTemplate = querySingle(EmailTemplate, "WHERE name='{$name}'");

	if($emailTemplate)
	{
		trace("Sending email '$name'", 3);
		$emailManager = new EmailManager($sendingItem, $emailTemplate);
		$rtn = $emailManager->sendEmail();
		trace("EmailManager:: rtn code $rtn", 3);
	}
}

?>
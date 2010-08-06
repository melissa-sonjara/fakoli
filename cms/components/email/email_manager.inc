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



//require_once  $config['homedir']."/cms/datamodel/survey.inc";

class EmailHandler
{
	var $toAddr;
	var $subject;
	var $message;
	var $displayText;
	var $headers;

	function EmailHandler($toAddr, $subject, $message = "", $emailFrom = "")
	{
		global $config;

		$this->toAddr = $toAddr;
		$this->subject = $subject;
		$this->displayText = $displayText;

		if(!$emailFrom) $emailFrom =  $config["email_from"];

		// Use the MD5 algorithm to generate a random hash
		$random_hash = md5(date('r', time()));

		$this->headers = "From: $emailFrom\r\n";
		$this->headers .= "Return-Path: ". $emailFrom . "\r\n";
		$this->headers .= "Reply-To: ". $emailFrom . "\r\n";
		$this->headers .= "Mime-Version: 1.0\r\n";

		if($config["alt_text_email"] == false)
			$this->headers .= "Content-type: text;\r\n";
		else
			$this->headers .= "\r\nContent-Type: multipart/alternative; boundary=\"PHP-alt-".$random_hash."\"";

		if($message AND $config["alt_text_email"])
			$this->message = $this->getMessageScript($message, $random_hash);
		elseif($message)
		{
			$patterns = array("/<br>/", "/&nbsp;/");
			$replacements = array("\r\n", " ");
			$message =  preg_replace($patterns, $replacements, $message);
			$this->message = stripHTML($message);
		}
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
		return mail($this->toAddr, $this->subject, $this->message, $this->headers);
	}

}



class EmailManager
{
	var $template;
	var $item; // dataitem object of the base class sending the emails
	var $message;
	var $subject;
	var $emailFrom;

	function EmailManager($item, $template_id, $emailFrom = "", $message = "")
	{
		$this->item = $item;
		$this->template_id = $template_id;
		$this->template = new EmailTemplate($template_id);
		$this->emailFrom = $emailFrom;
		$this->message = $message;

		$this->mergeEmail();
	}


	function mergeEmail()
	{
		$merges = $this->item->EmailCodeMerges();
		
		//echo "<pre>"; print_r($this->template); echo "</pre>";
		//echo "<pre>"; print_r($merges); echo "</pre>";

		if(count($merges) > 0)
		{
			$patterns = array();
			$replacements = array();
				
			foreach($merges as $merge)
			{
				
				$replaceText = $this->getReplaceText($merge);
						
				if($replaceText)
				{
					array_push($patterns, "[$merge->name]");
					array_push($replacements, $replaceText);
				}
			}
		}

		//echo "<pre>"; print_r($patterns); echo "</pre>";
		//echo "<pre>"; print_r($replacements); echo "</pre>";
		$this->subject = $this->searchReplaceText($patterns, $replacements, $this->template->subject);
		$this->message = $this->searchReplaceText($patterns, $replacements, $this->message);
		if($this->template->hasField("message_footer"))
			$this->message .= $this->searchReplaceText($patterns, $replacements, $this->template->message_footer);
	}
	
	
	function getReplaceText($merge)
	{
		if(is_callable($merge->map))
			$replaceText = call_user_func($merge->map, $this->item);
		else
		{
			list($relation, $field) = explode(".", $merge->map);
				
			// if the value_obj is the same as calling class then just keep that
			$replaceObj = (get_class($this->item) == $relation) ? $this->item : $this->item->$relation();

			if(is_object($replaceObj) AND $merge->function)
				$replaceText = $replaceObj->$field();
			elseif(is_object($replaceObj))
			{
				$replaceText = $replaceObj->format("{".$field ."}");
			}
			else // function called from the sending class
			{
				$replaceText = $replaceObj;
			}
		}
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

	function sendEmail()
	{
		$email = new EmailHandler($this->item->email, $this->subject, $this->message, $this->emailFrom);

		$sent = $email->send();

		return $sent;
	}
}

?>
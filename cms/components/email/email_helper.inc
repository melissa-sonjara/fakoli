<?php
Fakoli::using("email");

class EmailHelper
{
	static function displaySendResultMessage($msg)
	{
		global $config;
		
		$class = "";
		if ($msg==1)
		{
			$text = "Your message has been sent.";
		}
		elseif($msg==2)
		{
			$class = "fail";
			$text = "The email could not be sent. Please contact <a href=\"mailto:{$config['email_contact']}\">{$config["email_name"]}</a> for help.";
		}	
		
		if($msg)
		{
			echo "<div id='message_send_result' class='{$class}'>{$text}</div>\n";
		}
	}
	
	static function buildContactUsForm($form)
	{
		global $user;
		
		if($user)
		{
			$form->data->set("full_name", $user->first_name . " " . $user->last_name);
			$form->data->set("email", $user->email);
			$form->readOnly("full_name", "email");				
		}
		
		$form->required("full_name", "email", "subject", "message");
		
		$form->submitLabel = "Send";
		$form->formCSS = "form";
		$form->regexp("email", "\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b", "Please supply a valid email address containing a domain name.");
		
		$topicList = ContactTopic::formatList();
		if(count($topicList) > 0)
		{
			$aboutSelect = new SelectFieldRenderer($form, "contact_topic_id" , "About", $topicList);
		}
		else 
		{
			$form->hide("contact_topic_id");
		}
		
		$form->customSaveHandler = array(EmailHelper, sendContactUs);
		
		return $form;
	}

	/**
	 * customSaveHandler callback for contact_us form.
	 * 
	 * Create a dummy EmailTemplate to store the email details and
	 * send using EmailManager.
	 * 
	 * @param obj $form
	 * @return boolean
	 */
	function sendContactUs($form)
	{	
		$contactUs =& $form->data;
		
		$email = new EmailTemplate();
		$email->sender_email = $contactUs->email;
		
		$email->recipients = Settings::getValue('email', 'email_from');
			
		$contactTopic = $contactUs->ContactTopic();
		if($contactTopic && $contactTopic->recipients)
		{
			$email->recipients = $contactTopic->recipients;
		}
	
		if($contactTopic)
		{
			$email->subject = $contactTopic->topic . " ";
		}
	
		$email->subject .= $contactUs->subject;
		$email->message = "Message From: {$contactUs->full_name} {$contactUs->email}</br></br>" . $contactUs->message;
		
		$mgr = new EmailManager($contactUs, $email);
		
		// can be empty
		$mgr->attachments = $contactUs->get("attachment");
		$rtn = $mgr->sendEmail();
		
		if(!$rtn)
		{
			$form->msg = "Email send failed.";
		}
		
		return $rtn;		
	}
}
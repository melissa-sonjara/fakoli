<?php

class EmailLog extends DataItem
{
	var $table = "email_log";
	var $primary_key = "email_log_id";
	
	var $fields = array(
			"email_log_id"		=>	Number,
			"sender_name"		=>	String,
			"sender_email"		=>	String,
			"recipient_name"	=>	String,
			"recipient_email"	=>	String,
			"subject"			=>	String,
			"message"			=>	HTML,
			"date_sent"			=>	Timestamp,
			);
	
	/**
	 * Record email sent in a log.
	 *
	 * @param obj $mgr EmailManager
	 * @param string $toAddr
	 * @param boolean $rtn
	 */
	static function logEmail($mgr, $toAddr, $rtn)
	{
		// don't save log if email not sent.
		if(!$rtn) return;
		
		if (Settings::getValue("email", "log_messages"))
		{
			$item = $mgr->item;
					
			$log = new EmailLog();
			$log->recipient_name = "";
			$log->recipient_email = $toAddr;
			$log->sender_email = $mgr->emailFrom;
			$log->sender_name = $mgr->emailName;
			$log->subject = $mgr->subject;
			$log->message = Settings::getValue("email", "log_message_bodies") ? $mgr->message : "";
			$log->save();
		}
	}	
}
<?php

class Mail
{
	var $server;
	var $port;
	var $from;
	var $to;
	
	var $subject;
	var $body;
	var $headers;
	
	function Mail($from, $to, $subject, $body = "", $headers = "")
	{
		$this->server = ini_get("SMTP");
		$this->port   = ini_get("smtp_port");
		
		$this->from = $from;
		$this->to = $to;
		$this->subject = $subject;
		$this->body = $body;	
		$this->headers = $headers;	
	}

	function send()
	{
	    $smtp = new SMTP;
	    //$smtp->do_debug = 1; # sets the amount of debug information we get
	
	    # connect to the smtp server
	    if(!$smtp->connect($this->server, $this->smtp_port)) 
	    {
	        die ("Error: " . $smtp->error["error"] . "\n");
	    }
	
	    # say hello so we all know who we are
	    $smtp->Hello();
	
	    # start the mail transaction with with the return path address
	    $smtp->Mail($this->from);
	
	    # state each recipient
	    # NOTE: this does _not_ add to: or cc: headers to the email
	    $smtp->Recipient($this->to);
	
	    # send the message including headers (headers must be followed by an
	    # empty newline
	    $smtp->Data("From: $this->from\n".
	                "To: $this->to\n".
	                $this->headers.
	                "Subject: $this->subject\n".
	                "\n".
	                "$this->body\n");
	
	    # the mail either been sent or failed either way we are done
	    $smtp->quit();
	    
	    return true;
	}
}
?>
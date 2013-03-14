<?php
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

class Mail
{
	var $server;
	var $port;
	var $from;
	var $to;
	var $fullFrom;
	
	var $subject;
	var $body;
	var $headers;
	
	function Mail($from, $to, $subject, $body = "", $headers = "", $fullFrom = "")
	{
		$this->server = ini_get("SMTP");
		$this->port   = ini_get("smtp_port");
		
		$this->from = $from;
		$this->to = $to;
		$this->subject = encode7bit($subject);
		$this->body = $body;	
		$this->headers = $headers;	
		$this->fullFrom = encode7bit($fullFrom);
	}

	function send()
	{
		$debug = Settings::getValue("email", "use_debugging_mode");
		$debugPath = Settings::getValue("email", "debugging_mode_output_path");
		
		if ($debug)
		{
			$fp = fopen($debugPath . DIRECTORY_SEPARATOR . $this->to, 'a');
			if($fp === FALSE)
			{
				trace("Mail: unable to open debug log file in $debugPath name {$this->to}", 3);
				return false;
			}
			
			$rtn = fwrite($fp, "From: $this->from\n".
	                "To: $this->to\n".
	                $this->headers.
	                "Subject: $this->subject\n".
	                "\n".
	                "$this->body\n\n");
			
			if($rtn === FALSE)
			{
				trace("Mail: unable to write to debug log file in $debugPath name {$this->to}", 3);
				return false;
			}
			
	        fclose($fp);
	        return true;
		}
		
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
	
	    if (!$this->fullFrom) $this->fullFrom = $this->from;
	    
	    # send the message including headers (headers must be followed by an
	    # empty newline
	    $smtp->Data("From: $this->fullFrom\n".
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
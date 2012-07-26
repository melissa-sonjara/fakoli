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

class ErrorLogManager
{
	static $enableLogging = false;
	static $loggingDestination = "";
	static $errorPageTemplate = "";
	
	static function setDefaults()
	{
		Settings::setDefaultValue("error_log", "enable_logging", 0, Boolean);
		Settings::setDefaultValue("error_log", "logging_destination", "Database", String, "", "", "Database\nLog File");
		Settings::setDefaultValue("error_log", "error_page_template",  "/fakoli/error.tpl", String,"Specify the file name of your custom error page template, if you are using one. Leave this blank to use the default error reporting template");
	}
	
	static function enableLogging()
	{
		ErrorLogManager::$enableLogging = Settings::getValue("error_log", "enable_logging");
		ErrorLogManager::$loggingDestination = Settings::getValue("error_log", "logging_destination");
		ErrorLogManager::$errorPageTemplate = Settings::getValue("error_log", "error_page_template");
		
		set_exception_handler(array("ErrorLogManager", "exceptionHandler"));
		
	}

	static function exceptionHandler($exception)
	{
		global $user;
		
		try 
		{
	    	//Getting exception information
			$errMsg = $exception->getMessage();
			$errLevel = $exception->getCode();
			$errFile = $exception->getFile();
			$errLine = $exception->getLine();
	        $uri = $_SERVER['REQUEST_URI'];
	        
	        
			if ($errLevel & (E_NOTICE | E_WARNING | E_USER_WARNING | E_STRICT)) return;
		
			while(ob_end_clean()); // Clear all output buffers.

			$log = new ErrorLog();
			$log->code = $errLevel;
			$log->message = $errMsg;
			$log->file = $errFile;
			$log->line = $errLine;
			$log->details = $exception->getTraceAsString();
			$log->user_id = $user ? $user->user_id : 0;
			$log->uri = $uri;
			$log->session = session_id();
									
			if (ErrorLogManager::$enableLogging)
			{
				try
				{
					switch(ErrorLogManager::$loggingDestination)
					{
					case "Database":
						

						$log->save();
						
						break;
						
					case "Log File":
						
						trace($errMsg, 2, $exception->getTrace());
						break;
						
					default:
						
						break;
					}
				}
				catch(Exception $e)
				{
					//echo $e->getMessage();
				}
				
			}
			
			$err = Fakoli::resolveResource(ErrorLogManager::$errorPageTemplate);
			$template = file_get_contents($err);
			$out = $log->format($template);
			echo $out;
		}
		catch (Exception $ex)
		{
			echo $ex->getMessage();
			//Do nothing. 
			//Avoiding error from error handling...
		}
	    
		/* Don't execute PHP internal error handler */
	    return true;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new ErrorLogUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>
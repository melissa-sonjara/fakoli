<?php
/**
 * Application configuration variables.
 */
$config = array(

	"sitename"		=> "%{SITENAME}",
	"homedir"		=> "%{HOMEDIR}",
	"uploadbase"	=> "%{UPLOADBASE}",
	"uploaddir" 	=> "documents", 			/* Directory for uploads */
	"email_from" 	=> "%{EMAIL_FROM}", 		/* From address for generated emails */
    "http_host"		=> "%{HTTP_HOST}",			/* URL for this server */

    "email_contact" => "%{EMAIL_CONTACT}",	 	/* Email Address for contact */
    "email_name" 	=> "%{EMAIL_NAME}", 		/* Name of contact */
    
    "trace_path"	=> "%{TRACE_PATH}",			/* File path to trace output log */
    "trace_level"	=> 3,						/* Debugging trace level (0-5) */
	"trace_detail"	=> TraceDetailHigh,			/* Detail level for trace output */

	// Application specific settings - migrate to ApplicationSettings?
	"enable_user_login"		=>	true,
	"allow_pdf_download"	=>  true,
	"thumbnail_size"		=>	75,
	"prettyURLs"			=>	true,
	"admin_SSL"				=>	false
);

$auto_form_defaults = array(

	"labelCSS"	=>	"label",
	"buttonCSS"	=>	"button",
	"componentPath"	=> "/fakoli"
);


require_once realpath(dirname(__FILE__))."/connection.inc";
require_once realpath(dirname(__FILE__))."/defaults.inc";
require_once "framework/functions.inc";

/**
 * Pull out a simple reference to the request method
 */
$method = $_SERVER["REQUEST_METHOD"];
?>

<?php
/**
 * Application configuration variables.
 */
$config = array(

	"sitename"		=> "Sonjara CMS Website",
	"homedir"		=> "C:/code/cms",
	"uploadbase"	=> "C:/uploads/cms",
	"uploaddir" 	=> "documents", 			/* Directory for uploads */
	"tmp"			=> "/tmp",					/* Scratch Directory */
	"email_from" 	=> "andy@sonjara.com", 		/* From address for generated emails */
    "http_host"		=> "cms",		/* URL for this server */

    "email_contact" => "andy@sonjara.com", 		/* Email Address for contact */
    "email_name" 	=> "Andy Green", 			/* Name of contact */
    
    "trace_path"	=> "/share/cms_trace.log",	/* File path to trace output log */
    "trace_level"	=> 3,						/* Debugging trace level (0-5) */
	"trace_detail"	=>	TraceDetailHigh,		/* Detail level for trace output */

	"admin_templates_dir"	=>	"C:/code/cms/cms/admin/templates",

	// Application specific settings
	"latest_addition_count"	=>	3,
	"news_item_count"		=>	3,
	"enable_user_login"		=>	true,
	"allow_pdf_download"	=>  true,
	"thumbnail_size"		=>	75
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
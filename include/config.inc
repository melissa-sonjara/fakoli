<?php
/**
 * Application configuration variables.
 */
$config = array(

	"sitename"		=> "Sonjara CMS Website",
	"homedir"		=> "/srv/www/vhosts/cms.sonjara.com",
	"uploadbase"	=> "/srv/www/uploads/cms.sonjara.com",
	"uploaddir" 	=> "documents", 			/* Directory for uploads */
	"tmp"			=> "/tmp",					/* Scratch Directory */
	"email_from" 	=> "andy@sonjara.com", 		/* From address for generated emails */
    "http_host"		=> "cms.sonjara.com",		/* URL for this server */

    "email_contact" => "andy@sonjara.com", 		/* Email Address for contact */
    "email_name" 	=> "Andy Green", 			/* Name of contact */
    
    "trace_path"	=> "/share/cms_trace.log",	/* File path to trace output log */
    "trace_level"	=> 3,						/* Debugging trace level (0-5) */
	
	"admin_templates_dir"	=>	"/srv/www/vhosts/cms.sonjara.com/cms/admin/templates",

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
	"componentPath"	=> "/framework"
);


$admin_menu = array(

"Web Site Content"	=> array(

		"Sites"			=>	array("page"	=>	"/cms/admin/sites.php",
								  "role"	=>	"admin"),

		"Pages"			=>	array("page"	=>	"/cms/admin/pages.php",
								  "role"	=>	"admin"),
	
		"Menus"			=>	array("page"	=>	"/cms/admin/menus.php",
								  "role"	=>	"admin"),

		"Images"		=>	array("page"	=>	"/cms/admin/images.php",
								  "role"	=>	"admin"),
									  
		"Articles"		=>	array("page"	=>	"/cms/admin/news_items.php",
								  "role"	=>	"admin"),
									  
		"Documents"		=>	array("page"	=>	"/cms/admin/publications.php",
								  "role"	=>	"admin"),

		"Modules"		=>	array("page"	=>	"/cms/admin/modules.php",
								  "role"	=>	"admin")),

	
	"Classification"	=> array(

		"Groups"			=>	array("page"	=>	"/cms/admin/grps.php",
									  "role"	=>	"admin"),

		"Topics"			=>	array("page"	=>	"/cms/admin/topics.php",
									  "role"	=>	"admin")),

								  							  
	"User Management"	=>	array(

		"Users"				=>	array("page"	=>	"/cms/admin/users.php",
									  "role"	=>	"admin")),
	
							  							  
	"Public Site"		=>	array(
		"Home page"			=>	array("page"	=>	"/"))
);

require_once realpath(dirname(__FILE__))."/connection.inc";
require_once realpath(dirname(__FILE__))."/defaults.inc";
require_once realpath(dirname(__FILE__))."/../framework/functions.inc";

/**
 * Pull out a simple reference to the request method
 */
$method = $_SERVER["REQUEST_METHOD"];
?>
<?php
/**************************************************************
 * 
 * Title: defaults.inc
 * 
 * 
 * author: Andy Green for Sonjara, Inc.
 * 
 * date: 2008
 * 
 ***************************************************************/

//$auto_form_defaults = array(

	/*"labelCSS"					=>	"label",
	"valueCSS"					=>	"value",
	"buttonCSS"					=>	"button",
	"formCSS"					=>	"form",
	"componentPath"				=> "/framework",
	"HTMLEditorHideStyleBar"	=>	true,
	"HTMLEditorDefaultWidth"	=>	"600px",
	"HTMLEditorCSSPath"			=>	"/css/richtext.css",
	"javascriptEnabled"			=>	true,
	"markRequiredFields"		=>	true
	
);*/

$html_editor_extra_buttons = array(
	

	array("name"	=>	"source_picker", 
		  "image"	=>	"/images/photos.gif", 
		  "tooltip"	=>	"Insert Photos", 
		  "url"		=>	"/components/source_picker/source_picker.php", 
		  "width"	=>	"500px",
		  "height"	=>	"670px")
	
);

define(admin_begin_page, $config['admin_templates_dir']."/begin_page.inc");
define(admin_end_page, $config['admin_templates_dir']."/end_page.inc");
define(admin_begin_popup, $config['admin_templates_dir']."/begin_popup.inc");
define(admin_end_popup, $config['admin_templates_dir']."/end_popup.inc");

require_once realpath(dirname(__FILE__))."/functions.inc";
?>
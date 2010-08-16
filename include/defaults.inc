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
	

	array("name"	=>	"image_picker",
		  "image"	=>	"/cms/images/photos.gif", 
		  "tooltip"	=>	"Insert Photos", 
		  "url"		=>	"/cms/components/image_picker/image_picker.php",
		  "width"	=>	"500px",
		  "height"	=>	"670px")
	
);

require_once realpath(dirname(__FILE__))."/functions.inc";
?>
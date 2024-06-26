<?php
/**\defgroup email Email
 * @{
 */


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

class EmailManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "email";
		$component->description = "Email management for Fakoli/CMS";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 3;
		$component->enabled = true;
		
		return $component;
	}
	
	static function getAdminMenu()
	{
		return array
		(
			"Email Management"	=> array
			(

				"Email Templates"	=>	array("page"	=>	"/admin/email_template_list",
											  "role"	=>	"admin",
											  "weight"	=>	20,
											  "icon"	=>	"envelope-square"),
			
				"Merge Codes"	=>	array("page"	=>	"/admin/merge_code_list",
										  "role"	=>	"admin",
										  "weight"	=>	21,
										  "icon"	=>	"at"),
				
				"Email Logs"	=>	array("page"	=>	"/admin/email_logs",
										  "role"	=>	"admin",
										  "weight"	=>	22,
										  "icon"	=>	"archive"),
			
			),
			
			"Collaboration"	=> array
			(
				"Contact Topics"	=>	array("page"	=>	"/admin/contact_topics",
											  "role"	=>	"admin",
											  "weight"	=>	25,
											  "icon"	=>	"comment"),
				
				"Contact Us Log"	=>	array("page"	=>	"/admin/contact_us_log",
											  "role"	=>	"admin",
											  "weight"	=>	26,
											  "icon"	=>	"archive")
			),
				
			"Developer Tools"	=>	array
			(
				"Send Test Email"	=>	array("page"	=>	"/admin/test_email_form",
											  "role"	=>	"admin",
											  "weight"	=>	4,
											  "icon"	=>	"share-square"),

				"Email Debug Logs"	=>	array("page"	=>	"/admin/email_debug_logs",
											  "role"	=>	"admin",
											  "weight"	=>	5,
											  "icon"	=>	"archive")
			)	
		);
	}

			
	static function subscribeToEvents()
	{
		return array("ComponentScanComplete" 		=>	array(EmailManager, setDefaults),
					"upgradeComponent"				=>	array(EmailManager, upgradeComponent),
					"RegisterSerializationHandler"	=>	array(EmailManager, registerSerializationHandler),
					"RegisterEmailTransport"			=>	array(EmailManager, registerSMTPTransport),
					"RegisterSettingsFormExtension"	=>	array(EmailManager, registerSettingsFormExtension)
		
		);
	}
	
	static function getScripts()
	{
		return array("/components/email/js/email.js", "/components/email/js/email_attachment_manager.js");
	} 	
	
	static function getStyles()
	{
		return array("/components/email/css/email.css");
	}	
}


/**@}*/?>
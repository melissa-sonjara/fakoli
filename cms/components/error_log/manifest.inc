<?php
/**\defgroup error_log Error Log
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

class ErrorLogManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "error_log";
		$component->description = "Error logging and improved error display management for Fakoli/CMS";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 1;
		$component->enabled = true;
		
		return $component;
	}
	
	
	static function getAdminMenu()
	{
		return array
		(
			"User Management"	=> array
			(

				"Error Log"	=>	array("page"	=>	"/admin/error_log",
									  "role"	=>	"admin",
									  "weight"	=>	25,
									  "icon"	=>	"exclamation-circle")
			)
		);
	}	

			
	static function subscribeToEvents()
	{
		return array("ComponentScanComplete" =>	array(ErrorLogManager, setDefaults),
					 "RegisterErrorHandler"	=>	array(ErrorLogManager, enableLogging),
					 "upgradeComponent"		=>	array(ErrorLogManager, upgradeComponent)
		
		);
	}	
}


/**@}*/?>
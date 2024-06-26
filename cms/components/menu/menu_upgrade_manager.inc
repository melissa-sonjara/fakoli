<?php
/**\addtogroup menu
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

/*
 * Manage component upgrades/updates to Menu
 * component.
 * 
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *   
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 6/2/2011
 * 
 */

Fakoli::using("menu", "component");

class MenuUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
			"1.0"		=>	"sql:Initial Installation",
			"1.1"		=>	"sql:Added permissions field to menu_item",
			"1.2"		=>	"sql:Added required_parameters field to menu_item",
			"1.3"		=>	"sql:Added custom_format field to menu_item",
			"1.4"		=>	"sql:Added display_flags field to menu_item",
			"1.5"		=>	"sql:MenuItem page_id field defaults to zero now",
	        "1.6"		=>	"sql:MenuItem permissions field increased to Varchar(500)",
			"1.7"		=>	"sql:Added css_class field to menu_item",
			"1.8"		=>	"sql:Added container_css_class field to menu",
			"1.9"		=>	"sql:Added wrap_menu_items flag to menu"
	);
	
	function MenuUpgradeManager()
	{
		$this->component_name = "menu";

		// sets the sqlFile name according to default
		parent::UpgradeManager();		
	}
}
/**@}*/?>
<?php
/**\addtogroup report_manager
 * @{
 */


/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

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

Fakoli::using("report_manager", "component");

class ReportManagerUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
			"1.0"		=>	"done:Initial Installation",
			"1.1"		=>	"sql:Added table for persisting custom reports",
			"1.2"		=>	"sql:change configuration column to longtext; add column user_id",
			"1.3"		=>	"sql:Added 'shared' field to custom reports",
			"1.4"		=>	"sql:Added 'column_order' field to custom reports to allow custom column reordering"
	);
	
	function ReportManagerUpgradeManager()
	{
		$this->component_name = "report_manager";

		// sets the sqlFile name according to default
		parent::UpgradeManager();		
	}
		
	static function upgradeComponent($version)
	{
		$mgr = new ReportManagerUpgradeManager();
		$mgr->upgrade($version);
	}	
}
/**@}*/?>
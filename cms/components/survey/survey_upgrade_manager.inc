<?php
/**\addtogroup survey
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
 * Manage component upgrades/updates to Survey
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

Fakoli::using("survey", "component", "user");

class SurveyUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
			"1.0"		=>	"sql:Initial Installation",
			"1.1"		=>	"sql:restructure surveys into types; omit survey template",
			"1.2"		=>	"sql:Added context_help field to survey_question",
			"1.3"		=>	"sql:Add confirmation_message to survey table",
			"1.4"		=>	"sql:Added allow_anonymous_responses flag to survey table",
			"1.5"		=>	"sql:Added instructions field to survey table",
			"1.6"		=>	"sql:Added show_preview_before_submitting to survey table",
			"1.7"		=>	"sql:Add include_in_results to survey_response and to survey add additional_recipients and cc_recipients; add survey_group and survey_group_xref",
			"1.8"		=>	"sql:Change survey field status to varchar for sites setup prior to upgrade manager",
			);
	
	function SurveyUpgradeManager()
	{
		$this->component_name = "survey";

		// sets the sqlFile name according to default
		parent::UpgradeManager();		
	}
}
/**@}*/?>
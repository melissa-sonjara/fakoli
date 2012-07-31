<?php
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

class SurveyManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "survey";
		$component->description = "This component is generally used as a template for a developing customized surveys and survey response forms, and presentation of survey results. Custom implementation would override this component and interface with the survey_manager component to use classes that provide survey presentation support.";
		$component->author = "Janice Gallant";
		$component->version = "1.0";
		$component->priority = 5;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array
		(
		);
	}

	/*
	 * Note: if this manifest subscribes to events:
	 * QuestionnaireGetQuestion and QuestionnaireGetSurvey
	 * the graphs feature for any website that does NOT
	 * override the survey component will break.
	 */
	static function subscribeToEvents()
	{
		return array(
					 "EnumerateContextHelpClasses" => array(SurveyComponentManager, enumerateContextHelpClasses),
				 	"EnumerateDataItemClasses" => array(SurveyComponentManager, enumerateDataItemClasses),		
					"ComponentScanComplete" => array(SurveyComponentManager, setDefaults),
					"upgradeComponent"		=>	array(SurveyComponentManager, upgradeComponent)
		);
	}

	static function getStyles()
	{
		return array("/fakoli/css/survey.css");
	}


	static function getScripts()
	{
		return array("/components/survey/js/survey.js",
			);
	}
}

?>
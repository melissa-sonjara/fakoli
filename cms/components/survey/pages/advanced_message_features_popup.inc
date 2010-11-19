<?php
/**************************************************************
 * 
 * Title: survey_template_preview_popup.inc
 * 
 * Description: Displays view of a questionnaire in a popup.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/27/2010
 * 
***************************************************************/

Fakoli::using("survey");

$view = new MergeCodeView('SurveyResponse');

$script .= $view->writeScript();
$view->drawView();

?>
<?php
/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

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

 /* survey_list.inc
  *  
 * Description: Displays a list of survey email requests
 * and if it has been sent, the date sent.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/13/10
 */


Fakoli::using("questionnaire", "survey");
Fakoli::usingFeature("data_view");

$title = "My Surveys";

$constraint = "WHERE deleted=0 ";
if(!checkRole("admin,data"))
	$constraint .= "AND site_user_id={$user->site_user_id} ";
$surveys = groupedQuery(Survey, "$constraint  ORDER BY start_date DESC", "status");

$survey = new Survey(); // to get categories
$view = new StatusTrees($surveys, $survey->getStatusCategories(), SurveyStatusBlockView);
$view->width = '700';
$view->emptyMessage = "There are no surveys defined.";

$menuView = new SurveyMenuView('survey_list');
$survey_top_nav = $menuView->getView();
$styles .= "<link href=\"/templates/css/survey.css\" rel=\"stylesheet\"/>";
$styles .= "<link href=\"/templates/css/tree.css\" rel=\"stylesheet\"/>";
$styles .= "<link href='templates/css/survey.css' rel='stylesheet'/>\n";

?>
<div id="survey_tree">
<?
$view->drawView();
?>
</div>
<br/><br/>
<input type="button" class="button" value=" Add a Survey" onclick="go('survey_form');">

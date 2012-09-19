<?php
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

Fakoli::using("report_manager");

$report_id = $_GET["report_id"];

if ($report_id && is_numeric($_GET["report_id"]))
{
	try
	{
		$report = new CustomReport($report_id);
	}
	catch(Exception $e)
	{
		ajaxReturn($e->getMessage());
	}
}
else
{
	$report = new CustomReport();
	if (startsWith($report_id, "tmp-"))
	{
		$report->configuration = $_SESSION[$report_id];
		$report->manager_class = $_SESSION[$report_id . "-class"];
		$report_id = 0;
		$report->report_id = 0;
		$report->user_id = $user->get($user->getPrimaryKey());		
	}	
}


$form = new AutoForm($report, "POST", "/action/report_manager/update_report?report_id=$report_id");

$form->required("title");
$form->hide("column_order", "configuration", "manager_class", "shared", "user_id");

$form->ajaxSubmit("function(result) {ReportManager.reportSaved(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
	
$form->submitLabel = "Save Report";
$form->allowDelete = $report->allowDelete();

if ($method == "POST")
{
	if($form->save())
		ajaxReturn("OK");
	else
		ajaxReturn($form->msg);
}

echo $form->writeScript();

$form->drawForm();

?>
<script type="text/javascript">
window.addEvent('domready', function()
{
	document.id('CustomReport_form_column_order').value = ReportManager.columnOrder;
});
</script>
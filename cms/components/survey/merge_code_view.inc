<?php
/*
 * Gets table list view of merge codes for popup dialog box.
 * 
 *  @author Janice Gallant for Sonjara, Inc.
 *  
 *  10/17/2010
 */


Fakoli::using("email");
Fakoli::usingFeature("data_view");

class MergeCodeView
{	
	var $table;
	
	function MergeCodeView($class_name = 'SurveyResponse')
	{
		$mergeCodes = query(MergeCode, "WHERE class_name='$class_name'");
		$table = new DataListView($mergeCodes, "mergeCodes");
		$table->column("Code Name", "{name}", false, "width: 30%")
	  		->column("Description", "{description}", false, "width: 70%");
	  	
		$table->emptyMessage = "There are no merge codes defined.";
		$table->sortable = false;
		$table->cssStyle = "width: 100%";
		
		$this->table = $table;	
	}
	
	function writeScript()
	{
		return $this->table->writeScript();
	}
	
	function drawView()
	{
		$this->table->drawView();	

?>
<p>Customize your survey email message using the following codes to insert survey data from the database. The codes will appear in [brackets] while you edit your email template and will be replaced by data values when you email your survey.</p>
<?php
$table->drawView();
?>
<p>Example:</p>
<p>Please take a few minutes to complete this short [program_name] survey. Your feedback will help us know our participants and improve our program.</p>
<p>The survey will be open for responses until [survey_end_date]. We appreciate your support. </p>
<?php		
	}
	
	static function getView($mergeCodes)
	{
		$table = new DataListView($mergeCodes, "mergeCodes");
		$table->column("Code Name", "{name}", false, "width: 30%")
	  		->column("Description", "{description}", false, "width: 70%");
	  	
		$table->emptyMessage = "There are no merge codes defined.";
		$table->sortable = false;
		$table->cssStyle = "width: 100%";

?>
<p>Customize your survey email message using the following codes to insert survey data from the database. The codes will appear in [brackets] while you edit your email template and will be replaced by data values when you email your survey.</p>
<?php
$table->drawView();
?>
<p>Example:</p>
<p>Please take a few minutes to complete this short [program_name] survey. Your feedback will help us know our participants and improve our program.</p>
<p>The survey will be open for responses until [survey_end_date]. We appreciate your support. </p>
<?php		
		return $table;
	}
}


class SurveyMergeCodeDialog
{
	function SurveyMergeCodeDialog()
	{
		
	}
	
	static function writeScript()
	{
		$mergeCodes = query(MergeCode, "WHERE class_name = 'SurveyResponse'");
			
		global $dialogs;
		ob_start();
?>
<div class="dialog" id="mergeCodePopup" style="width: 500px">
 <div class="dialog_header" id="mergeCodePopupHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeMergeCodePopup'">Close &times;</a></div>
   <span style="font-weight: bold">Merge Codes</span>
  </div>	
 </div>
 <div class="dialog_body">
<?
$table = MergeCodeView::getView($mergeCodes);
?>
  </div>
</div>
<?		
		$dialogs .= ob_get_contents();
		ob_end_clean();
		
		ob_start();
?>
<script type='text/javascript'>

var mergeCodePopupDialog;

function mergeCodePopup(id, source)
{
	var popup = $('mergeCodePopup');
	
	if (!mergeCodePopupDialog)
	{
		mergeCodePopupDialog = new FloatingDialog('mergeCodePopup', {'closeLink': $('closeMergeCodePopup'), 'position': 'absolute'});
	}

	mergeCodePopupDialog.targetID = id;
	mergeCodePopupDialog.top = source.getCoordinates().top - 250;
	mergeCodePopupDialog.left = source.getCoordinates().left - 104;
	mergeCodePopupDialog.show();
	
}


</script>
<?
		$script = ob_get_contents();
		ob_end_clean();
		$script .= $table->writeScript();
		return $script;
		
	} // end writeScript
}
?>
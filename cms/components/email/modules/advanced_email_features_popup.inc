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

/*
 * 
 * Description: Displays survey message advanced features in a
 * popup.
 * 
 * @author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/27/2010
 */

Fakoli::using("email");
Fakoli::usingFeature("data_view");

$class_name = mysql_escape_string($_REQUEST["class_name"]);

$page->page_title = "Advanced Features &ndash; Merge Codes";
$mergeCodes = query(MergeCode, "WHERE class_name='$class_name'");
$table = new DataListView($mergeCodes, "mergeCodes");
$table->column("Code Name", "{name}", false, "width: 30%")
		->column("Description", "{description}", false, "width: 70%");
	  	
$table->emptyMessage = "There are no merge codes defined.";
$table->sortable = false;
$table->cssStyle = "width: 100%";

$table->writeScript();

?>
<p>The following codes are available for the recipients, subject and message fields of this template:</p>
<?php
$table->drawView();	
?>
<p>Additional codes can be added by your systems administrator.</p>
<p>Example:</p>
<p>Please take a few minutes to complete this short [program_name] survey. Your feedback will help us know our participants and improve our program.</p>
<p>The survey will be open for responses until [survey_end_date]. We appreciate your support. </p>

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

Fakoli::using("email", "text_lookup");
Fakoli::usingFeature("data_view");

$class_name = mysql_escape_string($_REQUEST["class_name"]);
$classes = array();
$classes = ComponentManager::fireEvent("EnumerateDataItemClasses", $classes);

$page->page_title = "Advanced Features &ndash; Merge Codes";
$mergeCodes = Query::create(MergeCode, "WHERE class_name=:cn")
	->bind(":cn", $class_name)
	->execute();

if(array_search($class_name, $classes))
{
	$obj = new $class_name();	
	$mergeCodes = getDefaultCodes($obj, $mergeCodes);
}

$table = new DataListView($mergeCodes, "mergeCodes");
$table->column("Code Name", "{name}", false)
		->column("Description", "{description}", false)
;		
$table->emptyMessage = "There are no merge codes defined.";
$table->sortable = false;
$table->cssStyle = "width: 100%";

echo $table->writeScript();
?>
<p>The following codes are available for class <?php echo $class_name ?>:</p>
<?php
$table->drawView();
?>
<p>Additional codes can be added by your systems administrator.</p>
<p>Example:</p>
<p>Please take a few minutes to complete this short [program_name] survey. Your feedback will help us know our participants and improve our program.</p>
<p>The survey will be open for responses until [survey_end_date]. We appreciate your support. </p><br>

<?php 

/*
 * Any of the sending class's fields can be used as a merge
 * code w/o creating a merge_code record. Since primary or
 * foreign key fields are not useful as codes, omit these from 
 * the list. Also omit composite_class fields and passwords.
 */
function getDefaultCodes($obj, $mergeCodes)
{
	$fields = $obj->getFields();
	if(count($fields) > 0)
	{
		foreach($fields as $name => $type)
		{
			if(preg_match("/_id/", $name) || preg_match("/composite/", $name) 
				|| preg_match("/password/", $name) || $type == Boolean)
				continue;
			
			$mCode = new MergeCode();
			$mCode->name = $name;
			$mCode->map = "N/A";
			$mCode->description = $type ." field in ". get_class($obj);
			$mergeCodes[] = $mCode;
		}	
	}
	return $mergeCodes;
}

?>
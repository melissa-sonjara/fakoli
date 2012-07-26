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

/* 
 * Title: email_code_list.php
 * 
 * Description: table list view of email template
 * fields. For developers only - not for client
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 8/13/10

***************************************************************/


Fakoli::using("email");
Fakoli::usingFeature("grouped_data_view");

$menu_item = "Merge Codes";

$mergeCodes = groupedQuery(MergeCode, "ORDER BY name", "class_name");


$classes = indexedQuery(MergeCode, "ORDER BY class_name", "class_name");

$table = new GroupedDataListView($mergeCodes, "mergeCodes");
$table->column("Code Name", "<a href=\"/admin/merge_code_form?merge_code_id={merge_code_id}\">{name}</a>")
	  	->column("Description", "{description}")
		->column("Map", "{map}")
		;
			  	
$table->emptyMessage = "There are no merge codes defined.";
$table->mode = "tree";
$table->cssStyle = "width: 100%";

foreach(array_keys($classes) as $class_name)
{
	$table->group($class_name, $class_name);
}

$script .= $table->writeScript();

$table->drawView();
		
echo "<br/><br/><a class='button' href='/admin/merge_code_form'>Add a Merge Code</a>\n";
?>
<?php
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
Fakoli::usingFeature("data_view");

$mergeCodes = query(MergeCode);

$table = new DataListView($mergeCodes, "mergeCodes");
$table->column("Code Name", "<a href=\"/admin/merge_code_form?merge_code_id={merge_code_id}\">{name}</a>")
	  	->column("Description", "{description}")
		->column("Map", "{map}")
	  	->column("Calling Class", "{class_name}");
	  	
$table->emptyMessage = "There are no merge codes defined.";
$table->sortable = false;
$table->cssStyle = "width: 100%";

$script .= $table->writeScript();

$table->drawView();
		
echo "<br/><br/><a class='button' href='/admin/merge_code_form'>Add a Merge Code</a>\n";
?>
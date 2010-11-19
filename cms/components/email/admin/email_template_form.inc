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
 * Description: add/update/delete email messages
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 6/7/10
 *
 */

Fakoli::using("email");
Fakoli::usingFeature("auto_form");

$email_template_id = checkNumeric($_GET["email_template_id"]);

$emailTemplate = new EmailTemplate();
global $config;

if ($email_template_id)
{
	$emailTemplate->load($email_template_id);
	$title = "Edit Email Template";
}
else
{
	$title = "Add an Email Template";
	$emailTemplate->sender_email = $config["email_from"];
}

$form = new AutoForm($emailTemplate);

$form->annotate("recipients", "(separate email addresses with a comma ',')");
$form->annotate("subject", "Codes must be enclosed in brackets '[' and ']'.");
$form->annotate("message", "Codes must be enclosed in brackets '[' and ']'.");
$form->unique("name", "An email template already exists with that title. Please choose a different title.");
$form->required("name", "sender_email", "recipients", "subject", "message");
$form->button("Cancel", "/admin/email_template_list");

//$form->validator->add(new EmailListValidator("recipients", "Recipients"));

// Name is used to find the template - do not change once saved
if($emailTemplate->email_template_id)
	$form->readOnly("name");

$table = getMergeCodeTable($emailTemplate);

if ($method == "POST")
{
	if($form->save())
		redirect("/admin/email_template_list");
}

$script .= $form->writeScript();
$script .= $table->writeScript();
echo "<h3>This form allows you to create or edit an email template.</h3>\n";

if($emailTemplate->email_template_id)
{
	echo "<p>The following codes are available for the recipients, subject and message fields of this template:</p>\n";
	$table->drawView();
	echo "</p><p>Additional codes can be added by your systems administrator.</p>";
} 
$form->drawForm();


function getMergeCodeTable($emailTemplate)
{	
	if($emailTemplate->class_name)
		$mergeCodes = query(MergeCode, "WHERE class_name = '{$emailTemplate->class_name}' ORDER BY NAME");
		
	if(count($mergeCodes) > 0)
		$list = formatItems($mergeCodes, "{name}", ", ");

	$table = new DataListView($mergeCodes, "mergeCodes");
	$table->column("Code Name", "{name}")
	  		->column("Description", "{description}");
	  	
	$table->emptyMessage = "There are no merge codes defined for this template.";
	$table->sortable = false;
	$table->cssStyle = "width: 50%";
	
	return $table;
}
?>
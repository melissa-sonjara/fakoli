<?php
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

if ($email_template_id)
{
	$emailTemplate->load($email_template_id);
	$title = "Edit Email Template";
}
else
{
	$title = "Add an Email Template";
}

$form = new AutoForm($emailTemplate);

$form->annotate("recipients", "(separate email addresses with a comma ',')");
$form->annotate("subject", "Codes must be enclosed in brackets '[' and ']'.");
$form->annotate("message", "Codes must be enclosed in brackets '[' and ']'.");
$form->unique("name", "An email template already exists with that title. Please choose a different title.");
$form->required("name", "recipients", "subject", "message");
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
<?php
/**************************************************************
 * 
 * Title: admin/context_help_form.php
 * 
 * Description: Displays form for entering context-sensitive
 * help.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/09
 * 
 ****************************************************************/

require_once  "../../../include/config.inc";
require_once  $config['homedir']."/include/permissions.inc";
require_once  $config['homedir']."/datamodel/context_help.inc";
require_once  $config['homedir']."/framework/auto_form.inc";
require_once  $config['homedir']."/framework/tree.inc";


$title = "Context-Sensitive Help";
$page_role = "admin";


$help_key = checkNumeric($_GET["help_key"]);
$field = ($_GET["field"]);
$className = ($_GET["class"]);
$prettyClassName = ($_GET["prettyClassName"]);

if(!$help_key && (!$field || !$className))
	redirect("context_help_list.php");

$help = new ContextHelp();

if(!$help_key)
{
	$help->field = $field;
	$help->class_name = $className;
}
else
{
	$help->load($help_key);	
}

$form = new AutoForm($help);
$form->allowDelete = true;
$form->readonly("field", "class_name");
$redirect = "context_help_list.php";
$form->button("Cancel", $redirect);

if ($method == "POST")
{
	$form->save();
	redirect($redirect);
}


$script = $form->writeScript();

include_once admin_begin_page;

$form->drawForm();

include_once admin_end_page;
?>
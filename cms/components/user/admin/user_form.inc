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

Fakoli::using("user", "role");
Fakoli::usingFile("framework/auto_form.inc");

$menu_item = "Users";
$user_id = checkNumeric($_GET["user_id"]);

$siteUser = new SiteUser();

$form = new AutoForm($siteUser);
$roleSelect = new ChecklistFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());
$roleSelect->setSize(323, 120);

$redirect = "/admin/users";
$form->unique("email", "That email address is already in use for another user");
$form->hide("composite_class", "language");
$form->button("Cancel", $redirect);

//$languageSelect = new SelectFieldRenderer($form, "language", "Language", SiteUser::$languageOptions);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);	
	}
}

if ($user_id)
{
	$siteUser->load($user_id);
	$title = "Edit Account for {$siteUser->first_name} {$siteUser->last_name}";
}
else
{
	$title = "Add a New User Account";
}

$script .= $form->writeScript();



$form->drawForm();


?>

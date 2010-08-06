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
 * 
 * Title: myprofile.php
 * 
 * Description: Enables users to update their user
 * profile.
 * 
 * author: Andy Green for Sonjara, Inc.
 * 
 * date: 2008
 *
 */
 
require_once realpath(dirname(__FILE__)."/../../include/config.inc");
require_once realpath(dirname(__FILE__)."/../../datamodel/site_user.inc");
require_once realpath(dirname(__FILE__)."/../../include/permissions.inc");
require_once realpath(dirname(__FILE__))."/../../framework/auto_form.inc";

$reset = checkNumeric($_GET["reset"]);

$profile = clone($user);

$profile->filter = new ExclusionFilter("role", "title", "activt");
$form = new AutoForm($profile);
$form->passwordEncryptor = hashPassword;

$form->submitLabel = "Update Profile";

$form->required("l_name", "f_name");
$form->unique("email", "An account already exists with that email address.");
$form->onSaveComplete = updateProfile;

if($reset == 1)
	$form->getRenderer("password")->forceReset = true;
	
if ($method == "POST")
{
	if ($form->save())
	{
		redirect("index.php");
	}
}

if($reset == 1)
	$form->msg = "You must reset your password. You will not be able to login with your token again.";
elseif($reset == 2)
	$form->msg = "Profile Updated.";

$script .= $form->writeScript();

$script .= "<link type='text/css' rel='stylesheet' href='/css/tree.css'/>";

require_once realpath(dirname(__FILE__)."/../../templates/begin_page.inc");

$form->drawForm();

require_once realpath(dirname(__FILE__)."/../../templates/end_page.inc");

function updateProfile($obj)
{
	global $user;
	global $_SESSION;
	global $reset;
	
	// reset session user in case of change
	$_SESSION["user"] = $obj->data;
	$user = $_SESSION["user"];
		
	// if user is resetting their profile, refresh and show it is updated
	if($reset)
		redirect("/components/login/myprofile.php?reset=2");
}

?>
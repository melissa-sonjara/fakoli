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

Fakoli::using("user", "email", "site");
Fakoli::usingFeature("auto_form");

$msg = checkNumeric($_GET["msg"]);
$identifier = $_REQUEST["identifier"];

$contactUs = new ContactUs();

if($user)
{
	$contactUs->full_name = $user->first_name . " " . $user->last_name;
	$contactUs->email = $user->email;
}


$form = new AutoForm($contactUs);
$form->required("full_name", "email", "subject", "message");
	
if($user)
{
	$form->readOnly("full_name", "email");
}

$form->submitLabel = "Send";
$form->formCSS = "form";
$form->regexp("email", "\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b", "Please supply a valid email address containing a domain name.");
$topicList = ContactTopic::formatList();
if(count($topicList) > 0)
	$aboutSelect = new SelectFieldRenderer($form, "contact_topic_id" , "About", $topicList);
else
	$form->hide("contact_topic_id");

if ($msg==1) 
{  
	echo "<p>Your message has been sent.</p>";
}
elseif($msg==2)
{ 
	echo"<p>The email could not be sent. Please contact <a href=\"mailto:{$config['email_contact']}\">{$config["email_name"]}</a> for help.</p>";
}


if ($method == "POST") 
{
	$contactUs =& $form->data;
	$contactUs->fromPOST();

	$contactTopic = $contactUs->ContactTopic();

	if(!$contactTopic OR ($contactTopic AND !$contactTopic->recipients))
		$recipients = array($config["email_contact"]);
	else
		$recipients = explode(",", $contactTopic->recipients);

	$subject = $contactTopic->topic . ": ". $contactUs->subject;
	
	foreach($recipients as $recipient)	
		$handler = new Mail($contactUs->email, $recipient, $subject, $contactUs->message);
			
	$rtn = $handler->send();
	$msg = ($rtn==true) ? 1 : 2;
	redirect("/$identifier?msg=$msg");
}


$script .= $form->writeScript();

if(!$msg)
{
	$form->drawForm();
}
?>

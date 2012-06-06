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
 * forum_request_form.inc
 * 
 * Description: Mail form for users with "member" role to request
 * a new type of discussion forum.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 7/12/10
 */

require_once $config['homedir']."/cms/datamodel/forum.inc";
require_once $config['homedir']."/framework/auto_form.inc";
require_once  $config['homedir']."/cms/components/mail/email_manager.inc";

$msg = checkNumeric($_GET["msg"]);

$forumRequest = new ForumRequest();
$forumRequest->to = $config["email_tech"];
$forumRequest->subject = "New Forum Request";

$form = new AutoForm($forumRequest);
$form->readonly("to", "subject");
$form->submitLabel = "Email Technical Support";
$form->button("Cancel", "forum_list");
$form->annotate("message", "Please specify a suggested title for the new forum and a description of its intended topic area.");

if ($method == "POST")
{
	$emailHandler = new EmailHandler($form->data->to, $form->data->subject, $_POST["message"], $user->email);
	$sent = $emailHandler->send();
	$msg = ($sent > 0) ? 1 : 2;
	redirect("forum_request_form?msg=$msg");
}

$script .= $form->writeScript();
if(!$msg)
	$form->drawForm();
else
{
	echo $forumRequest->getInfoText($msg);
?>
<button onclick="go('forum_list')" class="button">Return to Discussion Forums</button>
<?
} 
?>
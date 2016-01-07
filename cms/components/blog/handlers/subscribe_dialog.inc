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

Fakoli::using("blog", "text_lookup", "settings");
Fakoli::usingFeature("auto_form");

$blog_id = checkNumeric($_GET["blog_id"]);

$blogSubscriber = new BlogSubscriber();

$blogSubscriber->blog_id = $blog_id;
$blogSubscriber->subscription_type = 'instant';

$page->page_title = "Subscribe to Updates";

$form = new AutoForm($blogSubscriber, "POST", "/action/blog/subscribe_dialog?blog_id={$blog_id}");
$form->ajaxSubmit("function(response) { new BlogManager().subscriptionDialogResponse(response); }");

$form->hide("blog_id");
$form->regexp("subscriber_email", "\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,4}\b", "Please supply a valid email address containing a domain name.");
$form->required("subscriber_email");

if(Settings::getValue("blog", "allow_subscription_options"))
{
	$typeSelect = new RadioButtonFieldRenderer($form, "subscription_type", "Subscription Type", BlogSubscriber::$subscriptionTypes);
	$form->required("subscription_type");
}
else
{
	$form->hide("subscription_type");
}
$form->customSaveHandler = array(BlogManager, saveBlogSubscription);

$form->submitLabel = "Subscribe";

if ($method == "POST")
{
	if (!$blogSubscriber->subscription_token) 
	{
		$blogSubscriber->subscription_token = BlogSubscriptionManager::generateToken();
	}
	
	if ($form->save())
	{
		sendEmailUsingEmailManager($blogSubscriber, 'blog_subscription');
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
		
}

$script .= $form->writeScript();

echo TextLookup::getText("blog_subscriber_form_instructions");
$form->drawForm();

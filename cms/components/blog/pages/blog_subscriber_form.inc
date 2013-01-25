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

Fakoli::usingFeature("auto_form");
Fakoli::using("blog, text_lookup");

$page->page_title = "Subscribe to News Updates";

$blog_id = checkNumeric($_GET["blog_id"]);

$blogSubscriber = new BlogSubscriber();

$blogSubscriber->blog_id = $blog_id;

$form = new AutoForm($blogSubscriber);
$form->hide("blog_id");

$form->alias(subscriber_email, "Email");
$form->regexp("subscriber_email", "\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,4}\b", "Please supply a valid email address containing a domain name.");
		
$form->submitLabel = "Subscribe";

if ($method == "POST")
{
	if ($form->save())
	{
		sendEmailUsingEmailManager($blogSubscriber, 'blog_subscription');
	}
	redirect("/blog_subscription_successful?blog_id={$blog_id}");
}

$script .= $form->writeScript();
?>

<div id="tab_border">
<?

$subscription_instructions = TextLookup::getText("blog_subscriber_form_instructions");

if ($subscription_instructions)
	echo $subscription_instructions;
else
	echo "<p>Enter your email address to receive an email notification when a new article is posted.</p>";
	
$form->drawForm();
?>
<br/>
</div>
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

Fakoli::using("blog", "text_lookup");
Fakoli::usingFeature("auto_form");

$blogSubscriber = new BlogSubscriber();

$page->page_title = "Subscribe to Updates";

$form = new AutoForm($blogSubscriber);
$form->required("blog_id", "subscriber_email");

$form->regexp("subscriber_email", "\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,8}\b", "Please supply a valid email address containing a domain name.");

$blogSelect = new RelatedItemSelectFieldRenderer($form, "blog_id", "Select a Blog to Subscribe", Blog, "ORDER BY title", "title", "blog_id");
$blogSelect->allowNone = true;

if (!Settings::getValue("blog", "enable_extended_subscription_fields"))
{
	$blogSubscriber->filter = new ExclusionFilter("first_name", "last_name", "organization");
}

$form->submitLabel = "Subscribe";

if ($method == "POST")
{
	if ($form->save())
	{
		sendEmailUsingEmailManager($blogSubscriber, 'blog_subscription');
	}
	redirect("/admin/blog_subscriber_form");
}

$script .= $form->writeScript();
?>

<div id="tab_border">

<?
echo TextLookup::getText("blog_subscriber_form_instructions");
$form->drawForm();
?>
<br/>
</div>
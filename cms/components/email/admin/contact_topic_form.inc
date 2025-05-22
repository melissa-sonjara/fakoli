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
 * Description: add/update/delete view for contact topic
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 11/18/10
 *
***************************************************************/


Fakoli::using("email", "site");
Fakoli::usingFeature("auto_form");

$contact_topic_id = checkNumeric($_GET["contact_topic_id"]);

$contactTopic = new ContactTopic();

if ($contact_topic_id)
{
	$contactTopic->load($contact_topic_id);
}
else
{
	$contactTopic->recipients = $config["email_from"];
	$contactTopic->sort_order = queryValue('ContactTopic', "MAX(sort_order)") + 1;
}

if(!$contactTopic->recipients)
	$contactTopic->recipients = $config["email_from"];
	
$form = new AutoForm($contactTopic);

$redirect = "/admin/contact_topics";
$form->required("topic");
$form->allowDelete = true;
$form->annotate("recipients", "(separate email addresses with a comma ',')");

$sites = query('Site', "ORDER BY site_name");
if(count($sites) > 1)
	$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Include in Sites", $sites, "site_name", ContactTopicSiteXref);
//$form->validator->add(new EmailListValidator("recipients", "Recipients"));

if ($method == "POST")
{
	if($form->save())
	{
		redirect("/admin/contact_topics");
	}
}

$script .= $form->writeScript();

$form->drawForm();
?>

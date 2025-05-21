<?php
/**************************************************************

 Copyright (c) 2013 Sonjara, Inc

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

Fakoli::using("email");

$contact_us_id = checkNumeric($_GET["contact_us_id"]);
if (!$contact_us_id)
{
	redirect("contact_us_log");
}

$contactUs = new ContactUS($contact_us_id);

$form = new AutoForm($contactUs);
$topicSelect = new RelatedItemSelectFieldRenderer($form, "contact_topic_id", "Topic", 'ContactTopic', "", "{topic}", "contact_topic_id");
$form->alias("full_name", "Sender Name");
$form->alias("email", "Sender Email");

$form->readonly("full_name", "email", "contact_topic_id", "subject", "message");
$form->submitLabel = " &laquo; Back to Contact Us Log ";
$form->allowDelete = true;
if ($method == "POST")
{
	if ($form->save())
	{
		redirect("contact_us_log");
	}
}
?>
<h4>Received <?echo $contactUs->date_sent?></h4>
<?
$script .= $form->writeScript();
$form->drawForm();


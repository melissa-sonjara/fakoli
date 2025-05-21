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
 * Create a clone of the given email template.
 * 
 */

Fakoli::using("email");


$email_template_id = checkNumeric($_GET["email_template_id"]);

if($email_template_id)
{
	$emailTemplate = new EmailTemplate($email_template_id);
	$clone = clone($emailTemplate);
	
	do
	{
		$clone->name = "copy_of_" . $clone->name;	
	} while (!isUnique($clone->name));
	
	$clone->email_template_id = null;
	$clone->save();
}

redirect("/admin/email_template_form?email_template_id={$clone->email_template_id}");

function isUnique($name)
{
	$count = Query::create('EmailTemplate', "WHERE name = :n")
		->bind(":n", $name)
		->executeValue("COUNT(1)");

	return ($count > 0) ? false : true;
}

?>

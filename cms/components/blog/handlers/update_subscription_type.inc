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

Fakoli::using("blog");
Fakoli::usingFeature("auto_form");

$blog_subscriber_id = checkNumeric($_GET["blog_subscriber_id"]);
$subscription_type = mysql_escape_string($_GET["subscription_type"]);

if(!$blog_subscriber_id)
{
	Fakoli::end("update blog subscription type - unknown blog_subscriber_id");
}

if(!$subscription_type)
{
	Fakoli::end("update blog subscription type - unknown subscription type");
}

$blogSubscriber = new BlogSubscriber($blog_subscriber_id);

$blogSubscriber->filter = new ExclusionFilter("first_name", "last_name", "organization");

if($subscription_type != "unsubscribe")
{
	$blogSubscriber->subscription_type = $subscription_type;
	$blogSubscriber->filter = new InclusionFilter("subscription_type");
	$blogSubscriber->save();
}
else
{
	$blogSubscriber->delete();
}

Fakoli::end("OK");

?>
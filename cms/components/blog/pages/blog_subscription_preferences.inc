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
 * Description: Allows users to edit their forum subscriptions.
 * 
 * @author: Janice Gallant for Sonjara, Inc.
 * 
 * 10/28/2010
 */

Fakoli::using("blog", "settings");
Fakoli::usingFeature("data_view");

$token = checkIdentifier($_GET["subscription_token"]);
if(!$token)
{
	redirect("/index");
}

$subscriber = BlogSubscriber::fromToken($token);

$subscriptions = Query::create(BlogSubscriber, "WHERE subscriber_email=:email")
	->bind(":email", $subscriber->subscriber_email)
	->execute();

$div_id = "blog_subscription_preferences";

$script .= BlogSubscriptionManager::writeSubscriptionPreferenceScript($div_id);

echo "<div id='{$div_id}'>";

if(count($subscriptions))
{
	echo "<p><b>You are subscribed to the following blogs:</b></p>\n";
	foreach($subscriptions as $subscription)
	{
		echo $subscription->format("<fieldset>\n<legend>{BlogSubscriptionManager::formatTitleLink}</legend>\n<div class='subscription_options'>{BlogSubscriptionManager::renderSubscriptionTypeSelect}</div>\n</fieldset>\n");
	}	
}
else
{
	echo "<p><i>You are not subscribed to any blogs.</i></p>\n";
}
echo "</div>\n";

?>
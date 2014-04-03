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

Fakoli::using("forum");
Fakoli::usingFeature("data_view");

if(!$user) return;

$subscriptions = query(ForumSubscription, "WHERE user_id = {$user->user_id}");

$table = new DataListView($subscriptions, "forumSubscriptions");
$table->column("Forum Title", "{Forum.title}", true, "width: 40%")
		->column("Topic Title", getTopicTitle, true, "width: 35%")
		->column("Subscription Type", array(SubscriptionManager, renderSubscriptionType), false, "width: 25%");
	
$table->emptyMessage = "You are not subscribed to any forums or topics.";
$table->sortable = true;
$table->pageSize = 10;
$table->filter = true;
$table->cssStyle = "width: 90%";
$script .= $table->writeScript();

if ($method == "POST") 
{
  	if(saveForm())
		redirect("forum_subscription_preferences");
}



?>
<form name='forum_subscriptions' method='POST' action=''><br>
<?php
$table->drawView();

if(count($subscriptions) > 0)
{
?>
<br>
<input class="button" type='submit' name='save' value='Save Preferences'/>
</form>
<?php 
}


function getTopicTitle($subscription)
{
	if($subscription->topic_id)
	{
		$title = $subscription->ForumTopic()->FirstPost()->title;
	}
	else
		$title = "All Topics in Forum";
		
	return $title; 	
}

function saveForm()
{
	global $_POST;
		
	$subscriptions = $_POST["subscription"];
	
	if(count($subscriptions) > 0)
	{
		foreach($subscriptions as $forum_subscription_id => $subscription_type)
		{
			$subscription = new ForumSubscription($forum_subscription_id);
			SubscriptionManager::updateSubscription($subscription, $subscription_type);	
		}
	}
	return true;
}
?>
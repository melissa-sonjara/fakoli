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

/**
 * Allow users to change subscription options.
 *
 */

Fakoli::using("forum");

$topic_id = checkNumeric($_GET["topic_id"]);
$forum_id = checkNumeric($_GET["forum_id"]);
$forum_subscription_id = checkNumeric($_GET["forum_subscription_id"]);

if(!$user)
{
	Fakoli::end("You must be logged in to change your forum subscription preferences.");
}

if(!$forum_id)
{
	Fakoli::end("Unable to update subscription - unknown forum");
}

$forum = new Forum($forum_id);

$forumSubscription = new ForumSubscription();

$pk = $user->getPrimaryKey();
$forumSubscriptions = Query::create(ForumSubscription, "WHERE user_id=:user_id AND forum_id=:forum_id")
	->bind(":user_id", $user->$pk, ":forum_id", $forum_id)
	->execute();

$topicSubscriptions = array();
if(count($forumSubscriptions))
{
	foreach($forumSubscriptions as $fs)
	{
		if($fs->topic_id > 0)
		{
			$topicSubscriptions[] = $fs;
		}
	}
}

if($forum_subscription_id)
{
	$forumSubscription->load($forum_subscription_id);
}
else
{
	$pk = $user->getPrimaryKey();
	$forumSubscription->user_id = $user->$pk;
	$forumSubscription->forum_id = $forum_id;
	if($topic_id)
	{
		$forumSubscription->topic_id = $topic_id;
	}
	$forumSubscription->subscription_type = subscription_daily_digest;	
}

if($topic_id)
{
	// on save complete - determine if we are setting a topic subscription
	$forumSubscription->set("topics_page", true);
}
	
$topic = ($topic_id) ? new ForumTopic($topic_id) : null;
$helper = new ForumSubscriptionHelper($forum, $topic);

$form = new AutoForm($forumSubscription, "POST", "/action/forum/subscription_dialog?forum_id=$forum_id&topic_id=$topic_id&forum_subscription_id={$form->data->forum_subscription_id}");
$form->ajaxSubmit("function(result) {forumSubscriptionMgr.subscriptionDialogResult(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");

$form->hide("user_id", "forum_id", "topic_id");
$typeSelect = new RadioButtonFieldRenderer($form, "subscription_type", "", $helper->formatSubscriptionOptions());
$typeSelect->hideLabel = true;

$form->button("Cancel", "forumSubscriptionMgr.closeDialog()", null, true);
$form->customSaveHandler = array($helper, saveForumSubscription);
$form->labelCSS = "forum_subscription_label";

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

echo $form->writeScript();

echo $helper->drawHeaderText();
$form->drawForm();
?>
<?php
/*
 * Description: For each users who subscribes to one or more
 * forums, send an email with a summary of messages added to the 
 * topics to which they subscribe.
 * 
 * File is run dailey through windows scheduler.
 * 
 * Steps:
 * 
 * 1) Create an array of daily summaries for each forum topic.
 * 
 * 2) Get the set of forum subscripions grouped by user. For each
 * user, build the digest for each of their forum topic subscriptions.
 * 
 * 3) Save the email message into an array. Send all at the end.
 * 
 * @author: Janice Gallant for Sonjara, Inc.
 * 
 * 10/26/2010
 */


Fakoli::using("forum", "email", "user");

/*
 * Get all the new posts from today for each topic
 */
$today = date("Y-m-d");  

$messages = groupedQuery(ForumMessage, "WHERE last_modified like '$today%' ORDER BY last_modified", "topic_id");

if(count($messages) == 0)
	return;
	
$topics = indexedQuery(ForumTopic, "", "topic_id");

global $config;

/*
 * Create the digest for each topic in the forum
 */
if(count($messages) > 0)
{	
	foreach($messages as $topic_id => $topicMessages)
	{
		$firstPost = $topics[$topic_id]->FirstPost();
		$author = $firstPost->Author()->getFullName();
		$url = "http://" . $config["http_host"] . "/forum_topic?forum_id={$firstPost->forum_id}&topic_id={$firstPost->topic_id}"; 
		$forum_id = $firstPost->forum_id;
		$digest[$forum_id][$topic_id] .= "Forum Topic: \"{$firstPost->title}\" posted by {$author}\n";
		$digest[$forum_id][$topic_id] .= $url . "\n\n";
		foreach($topicMessages as $message)
		{
			$author = $message->Author()->getFullName();
			$digest[$forum_id][$topic_id] .= "Reply: \"{$message->title}\" posted by {$author}<br><br>";
		}	
	}
}

/*
 * Loop through the forum subscribers first by each forum,
 * so we can output the forum title at the start of the
 * topic group in the email message for each user.
 * 
 * Then, reindex the subscriptions to that forum by user_id.
 * Loop through each user subscription and add the digest 
 * for the topics to which they subscribe.
 * 
 * Store the digest messages in an array, indexed by the 
 * user_id.
 */


$forumKeys = implode(",", array_keys(reindexList($messages, "forum_id")));
$forums = indexedQuery(Forum, "WHERE forum_id IN ($forumKeys)", "forum_id");
$forumSubscribers = groupedQuery(ForumSubscription, "WHERE forum_id IN ($forumKeys) AND subscription_type = ".subscription_daily_digest, "forum_id");

echo "forum subscribers:<br>";
echo "<pre>"; print_r($forumSubscribers); echo "</pre>";

$forumTopics = reindexList($topics, "forum_id");

if(count($forumSubscribers) > 0)
{
	foreach($forumSubscribers as $forum_id => $forumSubscriptions)
	{
		$forum = $forums[$forum_id];
		$userSubscriptions = reindexList($forumSubscriptions, "user_id");

		foreach($userSubscriptions as $l_user_id => $subscriptions)
		{
			$digestMessages[$l_user_id] .= $forum->title . " Forum\n\n";
			// Whole forum subscriber
			if(!is_array($subscriptions))
			{
				$forumTopics = $forumTopics[$forum_id];
				foreach($forumTopics as $fTopic)
				{
					$digestMessages[$l_user_id] .= $digest[$forum_id][$fTopic->topic_id];
				}
			}
			else // individual topic subscriber
			{
				foreach($subscriptions as $subscription)
				{
					$digestMessages[$l_user_id] .= $digest[$forum_id][$subscription->topic_id];
				}
			}
		}
	}
}



/*
 * Finally, for each digestMessage in the array, send an
 * email to the user. Add a footer link to unsubscribe.
 */
//echo "<pre>"; print_r($digestMessages); echo "</pre>";

$mgr = new UserManager();
$userClass = $mgr->getUserClass();
$emailField = $mgr->getEmailField();
$userClassObj = new $userClass();
$pk = $userClassObj->getPrimaryKey();
$subscribers = indexedQuery($userClass, "WHERE $pk IN (SELECT user_id FROM forum_subscription)", $pk);
$subject = $config["sitename"] . " Daily Forum Digest"; 
$messageFooter = SubscriptionManager::formatUnsubscribeMessageFooter();

if(count($digestMessages) > 0)
{
	foreach($digestMessages as $l_user_id => $subscriberMessage)
	{
		$l_user = $subscribers[$l_user_id];
		$l_userEmail = $l_user->$emailField;
		$body = $subscriberMessage . $messageFooter;
		$email = new EmailHandler($l_userEmail, $subject, $body);
		$email->send();
	}
}

?>
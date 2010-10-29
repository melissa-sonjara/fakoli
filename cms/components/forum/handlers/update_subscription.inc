<?php
/*
 * Description: saves user selection of forum topics to
 * from the popup dialog on subscribe to on Forum Details page. 
 * 
 * Cases:
 * 
 * 1) User is subscribing to the forum as a whole. We would
 * have forum_id and subscription_type. Look for an existing
 * record in case they are changing their subscription type.
 * If they have any topic subscriptions in this forum, delete them.
 * 
 * 2) User is unsubscribing to the fourm as a whole. Delete
 * the subscription.
 * 
 * 3) User is subscribing to a topic. Create the subscription
 * record if it does not exist; update if it does.
 * 
 * 4) User is unsubscribing to a topic. Delete the subscription.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 10/25/2010
 */

Fakoli::using("forum");

$forum_id = checkNumeric($_GET["forum_id"]);
$subscription_type = checkNumeric($_GET["subscription_type"]);

if (!$forum_id OR !$subscription_type) die("");
trace("update_subscription:: not dead $forum_id $subscription_type", 3);

/*
 * Optional - topic_id is 0 if whole forum subscription;
 * forum_subscription_id is 0 if new subscripton
 */
$topic_id = checkNumeric($_GET["topic_id"]);
$forum_subscription_id = checkNumeric($_GET["forum_subscription_id"]);

if($forum_subscription_id)
	$subscription = new ForumSubscription($forum_subscription_id);

trace("update_subscription:: subscription {$subscription->forum_subscription_id}", 3);
	
trace("update_subscription:: " . subscription_unsubscribe, 3);
	
/*
 * Subscribe to entire forum, leave topic_id blank.
 * If user is subscribed to any topics within this forum,
 * delete the subscriptions.
 */ 

// update subscription type for either a forum or topic subscription
if($subscription)
	SubscriptionManager::updateSubscription($subscription, $subscription_type);	
// new forum subscription
elseif(!$subscription AND !$topic_id)
{
	trace("update_subscription:: new forum subscription", 3);
	// new subscription to a forum - delete any topic subscriptions
	// and save new forum subscription
	$pk = $user->getPrimaryKey();
	$topicSubscriptions = query(ForumSubscription, "WHERE user_id = {$user->$pk} AND forum_id={$forum_id}");

	if(count($topicSubscriptions) > 0)
	{
		foreach($topicSubscriptions as $topicSubscription)
			$topicSubscription->delete();
	}
	SubscriptionManager::saveSubscription($forum_id, 0, $subscription_type);
}
elseif($topic_id)
{	
	trace("update_subscription:: new topic subscription", 3);
	SubscriptionManager::saveSubscription($forum_id, $topic_id, $subscription_type);
}
else
{
	trace("update_subscription:: nothing to do", 3);
}

echo "1";
?>
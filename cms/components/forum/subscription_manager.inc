<?php
Fakoli::using("forum", "email");

class SubscriptionManager
{
	function SubscriptionManager()
	{
		
	}
	
	static function sendInstantNotifications($message)
	{
		global $config;
		
		// Don't query if this is a new topic.
		if($message->topic_id)
		{
			$constraint = "WHERE forum_id = {$message->forum_id} AND subscription_type = ". subscription_instant_notification ." AND (topic_id = 0 OR topic_id = {$message->topic_id})";
			$subscribers = query(ForumSubscription, $constraint);
		}

		if(count($subscribers) > 0)
		{
			$mgr = new UserManager();
			$userClass = $mgr->getUserClass();
			$emailField = $mgr->getEmailField();
			$emailContent = SubscriptionManager::formatTopicInstantNotification($message);
			$subject = $config["sitename"] . " Forum Instant Notification"; 
			trace("instant_notification:: email content $emailContent", 3);
	
			foreach($subscribers as $subscriber)
			{		
				$emailTo = $subscriber->User()->$emailField;
				$emailHandler = new EmailHandler($emailTo, $subject, $emailContent);
				$emailHandler->send();
			}
		}
	}
		
	
	static function formatTopicInstantNotification($message)
	{
		global $config;
		
		$firstPost = $message->Topic()->FirstPost();
		$url = "http://" . $config["http_host"] . "/forum_topic?forum_id={$firstPost->forum_id}&topic_id={$firstPost->topic_id}";

		$content = "Forum Topic: \"{$firstPost->title}\" posted by {$firstPost->Author()->getFullName()}\n";
		$content .= "http://" . $config["http_host"] . "/forum_topic?forum_id={$firstPost->forum_id}&topic_id={$firstPost->topic_id}\n\n";
		$content .= "Reply: \"{$message->title}\" posted by {$message->Author()->getFullName()}\n\n";
		$content .= strip_tags($message->message);  
		$content .= SubscriptionManager::formatUnsubscribeMessageFooter();  
		
		return $content;
	}
	
	static function formatUnsubscribeMessageFooter()
	{
		global $config;
		
		$messageFooter = "\n\nTo unsubscribe to these messages or change your subscription preferences, go to ";
		$messageFooter .= "http://" . $config["http_host"]  . "/forum_subscription_preferences\n\n";
		
		return $messageFooter;
	}
	
		
	/*
 	* Subscribe to entire forum, leave topic_id blank.
 	* If user is subscribed to any topics within this forum,
 	* delete the subscriptions.
 	*/ 
	
	static function updateSubscription($subscription, $subscription_type)
	{
		// update subscription type for either a forum or topic subscription
		if($subscription_type != subscription_unsubscribe AND
			$subscription_type != $subscription->subscription_type)
		{
			trace("update_subscription:: subscription change", 3);
			$subscription->filter = new InclusionFilter("subscription_type");
			$subscription->subscription_type = $subscription_type;
			$subscription->save();
		}
		elseif($subscription_type == subscription_unsubscribe)
		{
			trace("update_subscription:: unsubscribe", 3);
			$subscription->delete();
		}
	}
	
	
	static function saveSubscription($forum_id, $topic_id, $subscription_type)
	{
			
		global $user;
		$pk = $user->getPrimaryKey();
	
		$forumSubscription = new ForumSubscription();
		$forumSubscription->forum_id = $forum_id;
		$forumSubscription->topic_id = $topic_id; // subscribe to all
		$forumSubscription->user_id = $user->$pk;
		$forumSubscription->subscription_type = $subscription_type;
		$forumSubscription->save();
	}
}
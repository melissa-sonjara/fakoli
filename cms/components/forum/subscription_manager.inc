<?php
/**\addtogroup forum
 * @{
 */


Fakoli::using("forum", "email");

class SubscriptionManager
{
	function __construct()
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
			$emailField = $mgr->getEmailField();
			$emailContent = SubscriptionManager::formatTopicInstantNotification($message);
			$subject = $config["sitename"] . " Forum Instant Notification"; 
			trace("instant_notification:: email content $emailContent", 3);
	
			foreach($subscribers as $subscriber)
			{		
				$l_user = $subscriber->User();
				if($l_user)
				{
					$emailTo = $l_user->$emailField;
					$emailHandler = new EmailHandler($emailTo, $subject, $emailContent);
					$emailHandler->send();
				}
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
	
	static function renderSubscriptionType($subscription)
	{
		$options = SubscriptionManager::formatSubscriptionOptions($subscription->Forum(), $subscription);
		$current = $subscription->subscription_type;
		$forum_subscription_id = $subscription->forum_subscription_id;
	
		unset($options[0]);
		foreach($options as $value => $name)
		{
			$selected = ($current == $value) ? " checked" : "";
			$html .= "<input style='border: none' type='radio' id='subscription[{$forum_subscription_id}]' name='subscription[{$forum_subscription_id}]' value='$value' $selected>$name</option><br>\n";
		}
	
		return $html;
	}
	
	static function formatSubscriptionOptions($forum, $forumSubscription, $topicSubscriptionCount = 0)
	{
		$options = ForumSubscription::$subscriptionOptions;
	
		if($forumSubscription)
		{
			unset($options[0]);
		}
		
		if(!$forumSubscription && !$topicSubscriptionCount)
		{
			unset($options[subscription_unsubscribe]);
		}
	
		if($topicSubscriptionCount > 0)
		{
			$options[subscription_daily_digest] = "Subscribe to <b>" . $options[subscription_daily_digest] . "</b> for {$forum->title}";
			$options[subscription_instant_notification] = "Subscribe to <b>" . $options[subscription_instant_notification] . "</b> for {$forum->title}";
			$options[subscription_unsubscribe] .= " from topic(s) in {$forum->title}";
		}
	
		return $options;
	}
	
}


class ForumSubscriptionHelper
{
	var $forum;
	var $topic;
	var $title;
	var $topicSubscriptions;
	var $forumSubscription;
	var $topicSubscription;
	var $subscription;	///< if subscribed to the page you are on - forum subscription for forum page, 
					/// forum topic subscription for topic page
	var $topics_page = false;
	var $subscriptionOptions = array();
	
	var $icons = array(
		"forum" => "/fakoli/images/icon_subscribed_to_topic.png",
		"topic"	=> "/fakoli/images/icon_subscribed_to_topic.png");
	
	var $buttons = array(
			"forum" => "/fakoli/images/button_subscribe_forum.png",
			"topic"	=> "/fakoli/images/button_subscribe_topic.png");
	
	var $subscribeTypes = array("new_topic", "new_forum", "update_topic", "update_forum", "topic_to_forum");
	var $subscripeType = "";
		
	function __construct($forum, $topic = null)
	{
		$this->forum = $forum;	
		$this->topic = $topic;
		
		$this->forumSubscription = ForumSubscription::findUserSubscription($this->forum->forum_id, 0);
		
		if(!$this->topic)
		{
			$this->topicSubscriptions = ForumSubscription::getUserTopicSubscriptions($forum->forum_id);
			$this->subscription = $this->forumSubscription;
			if(!$this->forumSubscription && count($this->topicSubscriptions))
			{
				$this->subscribeType = "topic_to_forum";
			}
			else
			{
				$this->subscribeType = (!$this->forumSubscription) ? "new_forum" : "update_forum";
			}
		}
		else
		{
			$this->topics_page = true;
			$this->subscription = ForumSubscription::findUserSubscription($this->forum->forum_id, $this->topic->topic_id);
			$this->topicSubscription = $this->subscription;
			if($this->forumSubscription)
			{
				$this->subscribeType = "update_forum";
			}
			else
			{
				$this->subscribeType = ($this->topicSubscription) ? "update_topic" : "new_topic";
			}
		}
		
		$this->title = $this->getTitle();
	}	
	
	function getTitle()
	{
		if($this->topic)
		{
			$title =  $this->topic->getTitle();
		}
		else
		{
			$title = preg_replace("/forum$/i", "", $this->forum->title) . " Forum";
		}
		
		return $title;
	}
	
	
	/**
	 * On the forum or forum topic page, draw a link or button to allow
	 * access to dialog with subscription options.
	 */
	function drawLink()
	{
		global $user;
		if(!$user) return;
	
		$forum = $this->forum;
			
		$count = count($this->topicSubscriptions);
		
		if($this->subscribeType == "new_topic" || $this->subscribeType == "new_forum")
		{
			$this->drawButtonLink();
		}
		else if($this->subscribeType == "topic_to_forum")
		{
			$text = "You are Currently Subscribed to $count ";
			$text .= ($count > 1) ? "Topics" : "Topic";
			$text .= " in this Forum";
			$topOffset = ($count > 0) ? $count * 10 : 0;
			$this->drawIconLink($text, $topOffset);
		}
		else if($this->subscribeType == "update_forum" || $this->subscribeType == "update_topic")
		{
			$text = "You are Currently Subscribed to";
			if(!$this->topics_page)
			{
				$text .= " this Forum";
			}
			else if($this->topics_page && $this->forumSubscription)
			{
				$text .= " all Topics in this Forum";
			}
			else
			{
				$text .= " this Topic";
			}
			$this->drawIconLink($text);	
		}
	}
	
	function drawIconLink($text, $topOffset = 0)
	{
		$icon = ($this->topics_page) ? $this->icons["topic"] : $this->icons["forum"];
		$alt =  ($this->topics_page) ? "subscribe_topic" : "subscribe_forum";
		
		$img = "<img src='$icon' alt='{$alt}' style='border: none'>";
		echo "<p style=\"text-align: right\">$img&nbsp;&nbsp;<a id='subscription_link' href=\"#\" onclick=\"forumSubscriptionMgr.showSubscriptionDialog(); return false\">$text</a></p>\n";
	}
	
	function drawButtonLink($button, $topOffset = 0)
	{
		$button = ($this->topics_page) ? $this->buttons["topic"] : $this->buttons["forum"];
		
		$img = "<img src='$button' style='border: none'>";
		echo "<div style=\"display: inline-block; float: right\"><a id='subscription_link' href=\"#\" onclick=\"forumSubscriptionMgr.showSubscriptionDialog(); return false\">$img</a></div>\n";
	}

	function drawHeaderText()
	{
		if(!$this->topics_page)
		{
			$text = $this->formatForumSubscriptionHeader();
		}
		else
		{
			$text = $this->formatTopicSubscriptionHeader();
		}
	
		echo $text;
	}
	
	function formatForumSubscriptionHeader()
	{
		$subscription = $this->subscription;
		$count = count($this->topicSubscriptions);
		$subscribeForumText = "If you prefer, you can subscribe to the forum as a whole.";
		
		if(!$subscription AND $count == 0)
		{
			$text = "<p>You can subscribe to the forum's daily digest or subscribe to instant notification to receive an email whenever a new topic or reply is posted.</p>\n";
		}
		elseif(!$subscription AND $count > 0 AND $count < 20)
		{
			$text = "<p>You are currently subscribed to the following topic(s) in this forum:</p>";
			$text .= "<ul>\n" . formatItems($this->topicSubscriptions, "<li>{ForumSubscriptionHelper::formatTopicSubscription}</li>", "\n") . "</ul>\n";
			$text .= "<p>{$subscribeForumText}</p>";
		}
		elseif(!$subscription AND $count > 0)
		{
			$text = "<p>You are currently subscribed to several topics in this forum:</p>";
			$text .= "<p>{$subscribeForumText}</p>";
		}
		else if($subscription)
		{
			$subscriptionType = ForumSubscription::$subscriptionOptions[$subscription->subscription_type];
			$text = "<p>You are currently subscribed to $subscriptionType for this forum. You can change your subscription type or unsubscribe.</p>";
		}
		
		return $text;
	}
	
	static function formatTopicSubscription($subscription)
	{
		return $subscription->ForumTopic()->FirstPost()->title;
	}
	
	function formatTopicSubscriptionHeader()
	{
		$subscription = $this->topicSubscription;
		
		if(!$subscription)
		{
			return "<p>You can subscribe to the topics's daily digest or subscribe to instant notification to receive an email whenever a reply is posted.</p>\n";
		}
		else 
		{
			$subscriptionType = ForumSubscription::$subscriptionOptions[$subscription->subscription_type];			
			return "<p>You are currently subscribed to $subscriptionType for this topic. You can change your subscription type or unsubscribe.</p>";
		}
	}
	
	/**
	 * Cases:
	 * 1) On forum page and already subscribed to forum:
	 * allow change of subscription type and unsubcribe
	 * 
	 * 2) On forum page and not subscribed to forum or
	 * any topics
	 * allow choose subscription type for forum
	 * 
	 * 3) On forum page and already subscribed to one or more
	 * topics:
	 * allow change of subscription type and unsubscribe to 
	 * all topics in the forum
	 * 
	 * 4) On topic page and already subscribed to forum:
	 * allow change of subscription type and unsubscribe from forum
	 * 
	 * 5) On topics page and already subscribed to that topic
	 * allow change of subscription type and unsubscribe
	 * 
	 * @return array change of subscription type and unsubscribe
	 * 
	 * 5) On topics page and subscribed to a different topic in 
	 * that forum or no topics in that forum
	 * allow choose subscription type
	 */
	function formatSubscriptionOptions()
	{
		$options = ForumSubscription::$subscriptionOptions;
	
		$subscription = $this->subscription;
		$count = count($this->topicSubscriptions);
	
		$title = "<i>". $this->title . "</i>";
	
		$options[subscription_daily_digest] = "Subscribe to <b>" . $options[subscription_daily_digest] . "</b> for {$title}";
		$options[subscription_instant_notification] = "Subscribe to <b>" . $options[subscription_instant_notification] . "</b> for {$title}";
		
		$unsubscribeText = "Unsubscribe from";
		if($subscription)
		{
			$options[subscription_unsubscribe] = $unsubscribeText . " {$title}";
		}
		else if(!$subscription && !$this->topics_page && $count)
		{
			$options[subscription_unsubscribe] = $unsubscribeText . " from all topics in {$title}";
		}
	
		return $options;
	}
	
	/**
	 * customSaveHandler for ForumSubscription subscription_dialog
	 *
	 * @param obj $form
	 */
	function saveForumSubscription($form)
	{
		$subscription = $form->data;
		$type = $subscription->subscription_type;
	
		if($subscription->forum_subscription_id && $type != subscription_unsubscribe)
		{
			$subscription->filter = new InclusionFilter("subscription_type");
			$subscription->save();
		}
		else if($type != subscription_unsubscribe)
		{
			trace("**************forum saveForumSubscription type is not unsubscribe - save new subscription");
			if(!$subscription->topics_page)
			{
				$this->forumUnsubscribe();
			}
			$subscription->save();
		}
		else if($type == subscription_unsubscribe)
		{
			$this->forumUnsubscribe();
		}
		else
		{
			trace("update_subscription:: nothing to do", 3);
		}
	
		return true;
	}
	
	/**
	 * Unsubscribe from this form or any topics within this forum.
	 *
	 */
	function forumUnsubscribe()
	{
		trace("SubscriptionManager:: forumUnsubscribe", 3);
		global $user;
	
		if(!$user) return;
	
		$forumSubscription = new ForumSubscription();
		$pk = $user->getPrimaryKey();
		$user_id = $user->$pk;
		$forumSubscription->delete("WHERE forum_id={$this->forum->forum_id} AND $user_id={$user_id}");
	}
	
		
	
	function writeScript()
	{
		$forum_subscription_id = ($this->subscription) ? $this->subscription->forum_subscription_id : 0;
		$topic_id = $this->topic->topic_id ? $this->topic->topic_id : 0;

		ob_start();
	
		?>
	
		<script type="text/javascript" src="/components/forum/js/forum_subscription_manager.js"></script>
		<script type="text/javascript">
				
		var forumSubscriptionMgr;
				
		window.addEvent('domready', function()
		{
			forumSubscriptionMgr = new ForumSubscriptionManager(
			<?php echo $this->forum->forum_id ?>, <?php echo $topic_id ?>, <?php echo $forum_subscription_id ?>, '<?php echo $this->title ?>');
			});
				
		</script>
				
		<?php
		$script .= ob_get_contents();
		ob_end_clean();
						
		return $script;		
	}		
}

/**
 * On forum topics page, options are
 * 
 * 1) Not already susbcribed to topic or forum - subscribe to topic
 * 
 * 2) Update topic subscription
 * 
 * 3) Not subscribed to topic but are subscribed to forum - allow update
 * to forum subscription
 * 
 * @author janice
 *
 */
class ForumTopicSubscriptionHelper extends ForumSubscriptionHelper
{
	function __construct($forum, $topic)
	{
		$this->forum = $forum;
		$this->topic = $topic;
		
		$this->forumSubscription = ForumSubscription::findUserSubscription($this->forum->forum_id, 0);
		
		$this->subscription = ForumSubscription::findUserSubscription($this->forum->forum_id, $this->topic->topic_id);

		if($this->forumSubscription)
		{
			$this->subscribeType = "update_forum";
		}
		else
		{
			$this->subscribeType = ($this->subscription) ? "update_topic" : "new_topic";
		}
		
		$this->title = $this->topic->getTitle();
		
	}

	function drawLink()
	{
		global $user;
		if(!$user) return;
	
		if($this->subscribeType == "new_topic")
		{
			$this->drawButtonLink();
		}
		else if($this->subscribeType == "update_topic")
		{
			$text = "You are Currently Subscribed to";
			$text .= ($this->forumSubscription) ? " all Topics in this Forum" : " this Topic";
			$this->drawIconLink($text);
		}
	}
	
	function drawHeaderText()
	{
		echo $this->formatHeader();
	}
	
	function formatHeader()
	{
		if(!$this->subscription)
		{
			return "<p>You can subscribe to the topics's daily digest or subscribe to instant notification to receive an email whenever a reply is posted.</p>\n";
		}
		else
		{
			$subscriptionType = ForumSubscription::$subscriptionOptions[$this->subscription->subscription_type];
			return "<p>You are currently subscribed to $subscriptionType for this topic. You can change your subscription type or unsubscribe.</p>";
		}
	}
	
	
}
/**@}*/?>

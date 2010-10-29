<?php
/*
 * Presents a dialog on the forum_details and forum_topic pages
 * for users to subscribe, unsubscribe, or change subscription
 * type in the forums and topics.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 10/26/2010
 */


class ForumSubscriptionView
{
	var $forum;
	var $topics;
	var $subscription;
	var $popupTitle;
	var $topicSubscriptions;
	
	function ForumSubscriptionView($forum, $topics)
	{
		global $user;
		if(!$user)
			return;
		$pk = $user->getPrimaryKey();
		
		$this->forum = $forum;
		$this->topics = $topics;
		
		$subscriptions = query(ForumSubscription, "WHERE forum_id={$this->forum->forum_id} AND user_id = {$user->$pk} AND topic_id = 0");

		if(count($subscriptions) > 0)
			$this->subscription = $subscriptions[0];
			
		$pk = $user->getPrimaryKey();
		$this->topicSubscriptions = query(ForumSubscription, "WHERE user_id = {$user->$pk} AND forum_id={$this->forum->forum_id} AND topic_id > 0");
	
		$this->popupTitle = "{$this->forum->title} Forum";	
	}
	
	/*
	 * Same script is used for both forum subscription dialog and
	 * forum topic subscription dialog but the drawView functions
	 * are different.
	 */
	function writeScript()
	{
		global $user;
		if(!$user) return;
		global $dialogs;
		ob_start();
?>
<div class="dialog" id="subscribePopup" style="width: 320px">
 <div class="dialog_header" id="subscribePopupHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeSubscribePopup'">Close &times;</a></div>
   <span"style="font-weight: bold"><?php echo $this->popupTitle ?></span>
  </div>	
 </div>
 <div class="dialog_body">
<?
	$this->drawView();
  ?>
  <br><button class='button' style='float: right; margin-bottom: 4px' onclick='updateSubscription()'>Update Subscription</button>
 </div>
</div>
<?php 

		$dialogs .= ob_get_contents();
		ob_end_clean();
		
		ob_start();
?>
<script type='text/javascript'>

var subscribePopupDialog;

function subscribePopup(id, topic, source, topOffset)
{
	var popup = $('subscribePopup');
	
	if (!subscribePopupDialog)
	{
		subscribePopupDialog = new FloatingDialog('subscribePopup', {'closeLink': $('closeSubscribePopup'), 'position': 'absolute'});
	}

	subscribePopupDialog.targetID = id;
	subscribePopupDialog.top = source.getCoordinates().top - topOffset;
	subscribePopupDialog.left = source.getCoordinates().left - 204;
	subscribePopupDialog.show();
}


function updateSubscription()
{
	var id = subscribePopupDialog.targetID;
	var popup = $('subscribePopup');

	var subscription_type;
	var forum_subscription_id;

	/* Only used for topic subscription; 0 for forum subscription */
	var topic_id = popup.getElementById("topic_id").value;

	forum_subscription_id = popup.getElementById("forum_subscription_id").value;
	
	popup.getElements("input[type='radio']").each(function(e) 
			{ if (e.checked) 
				subscription_type = e.value;
			}
		);


	var request = new Request({
		url: "/action/forum/update_subscription?forum_id=" + id + "&subscription_type=" + subscription_type + "&topic_id=" + topic_id + "&forum_subscription_id=" + forum_subscription_id,
		method: 'get',
		onSuccess: function (response) 
		{
			if (response == 1)
			{
				window.location.reload();
			}
		},
		onFailure: function () {},
		async: true
	});
request.send();
}
</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		return $script;
	}
	
	function drawLink()
	{
		global $user;		
		if(!$user) return;
			
		$count = count($this->topicSubscriptions);
		if(!$this->subscription AND $count == 0)
			$text = "Subscribe to this Forum";
		elseif(!$this->subscription AND $count > 0)
		{
			$text = "You are Currently Subscribed to $count ";
			$text .= ($count > 1) ? "Topics" : "Topic";
			$text .= " in this Forum"; 
		}
		else
			$text = "You are Currently Subscribed to this Forum";

		$count = count($this->topicSubscriptions);
		$topOffset = ($count > 0) ? $count * 10 : 0;	
		echo "<p style=\"text-align: right\"><a id='subscription_link' href=\"#\" onclick=\"subscribePopup({$this->forum->forum_id}, 0, this, {$topOffset}); return false\">$text</a></p>\n";
		
	}
	
	function drawView()
	{
		global $user;
		if(!$user) return;

		$this->drawHeaderText();
	
		$this->renderFields($this->subscription, 0);
	}
	
	function drawHeaderText()
	{
		$subscription = $this->subscription;
		$count = count($this->topicSubscriptions);
		
		if(!$subscription AND $count == 0)
		{
			$text = "<p>You can subscribe to the forum's daily digest or subscribe to instant notification to receive an email whenever a new topic or reply is posted.</p>\n";
		}
		elseif(!$subscription AND $count > 0 AND $count < 20)
		{
			$text = "<p>You are currently subscribed to the following topic(s) in this forum:</p>";
			$text .= "<ul>\n";
			foreach($this->topicSubscriptions as $topicSubscription)
			{
				$text .=  "<li>". $topicSubscription->ForumTopic()->FirstPost()->title . "</li>\n";
			}
			$text .= "</ul>\n";
			$text .= "<p>If you choose to subscribe to the forum as a whole, your individual topic subscriptions will be deleted.</p>";
		}	
		elseif(!$subscription AND $count > 0)
		{
			$text = "<p>You are currently subscribed to several topics in this forum:</p>";
			$text .= "<p>If you choose to subscribe to the forum as a whole, your individual topic subscriptions will be deleted.</p>";
		}		
		
		else
		{
			$text = "<p>You are currently subscribed to this forum. You can change your subscription type or unsubscribe.</p>";
		}	
		
		echo $text;
		
	}
	
	/*
	 * subscription parameter may be null if no subscription
	 * 
	 * topic_id may be null if subscription to whole forum
	 */
	function renderFields($subscription, $topic_id)
	{
		$subscriptionTypes = $subscription->subscriptionTypes;
	
		$subsObj = new ForumSubscription();
		$subscriptionTypes = $subsObj->subscriptionTypes;
		
		$current = ($subscription) ? $subscription->subscription_type : subscription_daily_digest;
		
		foreach($subscriptionTypes as $value => $name)
		{
			$selected = ($current == $value) ? " checked" : "";
			echo "<input style='border: none' type='radio' id='subscription_type' name='subscription_type' value='$value' $selected>$name</option><br>\n";
		}
		
		if($subscription)
		{
			echo "<input style='border: none' type='radio' name='subscription_type' value='3'>Unsubscribe</option><br>\n";
		}
		
					
		$forum_subscription_id = ($subscription) ? $subscription->forum_subscription_id : 0;
		echo "<input type='hidden' id='forum_subscription_id' name='forum_subscription_id' value='{$forum_subscription_id}'/>\n";
		
		// Include so we can use same script for both forum and topic subscriptions - here value 0
		echo "<input type='hidden' id='topic_id' name='topic_id' value='{$topic_id}'/>\n";
		
		
	}
}	


class TopicSubscriptionView extends ForumSubscriptionView
{
	var $topic;
	var $topicSubscription;
	
	function TopicSubscriptionView($topic)
	{
		global $user;
		if(!$user) return;
		
		$pk = $user->getPrimaryKey();
		
		$this->topic = $topic;
		
		// First check if the user has subscribed to the forum in its entirety
		$subscriptions = query(ForumSubscription, "WHERE forum_id={$this->topic->forum_id} AND user_id = {$user->$pk} AND (topic_id is null OR topic_id = 0)");

		if(count($subscriptions) > 0)
		{
			$this->subscription = $subscriptions[0];
			$this->forum = new Forum($this->topic->forum_id);
			$this->topics = $this->forum->Topics();
			$this->popupTitle = "{$this->forum->title} Forum";
		}
		else
		{
			$topicSubscriptions = query(ForumSubscription, "WHERE forum_id={$this->topic->forum_id} AND user_id = {$user->$pk} AND topic_id = {$this->topic->topic_id}");
			if(count($topicSubscriptions) > 0)
				$this->topicSubscription = $topicSubscriptions[0];
			$this->popupTitle = "{$this->topic->FirstPost()->title} Topic";
		}
	}

	/*
	 * If the user has a subscription to this forum, then provide
	 * same link and options as if they were on the forum_details page.
	 * If the user is not subscribed to the forum, then if subscribed
	 * to the topic, show "You are subscribed to this topic" link with
	 * options to update subscription, else show "Subscribe to this
	 * topic" link with options to subscribe.
	 */
	function drawLink()
	{
		global $user;
		if(!$user) return;
		
		if($this->subscription)
		{
			$text = "You are Currently Subscribed to all Topics in this Forum";
		}
		else
		{
			if($this->topicSubscription)
				$text = "You are Currently Subscribed to this Topic";
			else
				$text = "Subscribe to this Topic";
		}
		
		$count = count($this->topicSubscriptions);
		$topOffset = ($count > 0) ? $count * 5 : 0;		
		echo "<p style=\"text-align: right\"><a id='subscription_link' href=\"#\" onclick=\"subscribePopup({$this->topic->forum_id}, 0, this, {$topOffset}); return false\">$text</a></p>\n";
			
	}
	
	function drawView()
	{
		global $user;
		if(!$user) return;

		// The user has a subscription to the whole forum and
		// cannot subscribe the the individual topic
		if($this->subscription)
		{
			$subscription = $this->subscription;
			parent::drawHeaderText();
			$topic_id = 0;
		}
		else // topic subscription
		{
			$subscription = $this->topicSubscription;
			$this->drawHeaderText();
			$topic_id = $this->topic->topic_id;		
		}

		$this->renderFields($subscription, $topic_id);
	}
	
	function drawHeaderText()
	{
		$subscription = $this->topicSubscription;
	
		if(!$subscription)
			echo "<p>You can subscribe to the topics's daily digest or subscribe to instant notification to receive an email whenever a reply is posted.</p>\n";
		else
			echo "<p>You are currently subscribed to this topic. You can change your subscription type or unsubscribe.</p>";
	}
}

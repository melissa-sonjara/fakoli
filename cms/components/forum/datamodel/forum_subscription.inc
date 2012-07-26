<?php
define("subscription_daily_digest", 1);
define("subscription_instant_notification", 2);
define("subscription_unsubscribe", 3);

class ForumSubscription extends DataItem
{
	var $fields = array(
			"forum_subscription_id"	=>	Number,
			"forum_id"				=>	Number,
			"topic_id"				=>	Number,
			"user_id"				=>	Number,
			"subscription_type"		=>	Number
	);
	
	var $relations = array(
			"Forum"			=>	Forum,
			"ForumTopic"	=>	ForumTopic,
			"User"			=>	""
	);
	
	var $subscriptionTypes = array(
			1		=>  "Daily Digest",
			2		=>	"Instant Notification"
	);
	
	
	function Forum()
	{
		return $this->getRelated(Forum);
	}
	
	function ForumTopic()
	{
		return $this->getRelated(ForumTopic);
	}
	
	function User()
	{
		$mgr = new UserManager();
		return $this->getRelated($mgr->getUserClass());
	}
	
	function ForumSubscription()
	{
		$this->primary_key = "forum_subscription_id";
		$this->table = "forum_subscription";

		$this->DataItem(func_get_args());
		
	}
}
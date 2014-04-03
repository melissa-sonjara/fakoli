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
	
	static $subscriptionOptions = array(
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
		return $this->getRelated($mgr->getUserClass(), "user_id");
	}
	
	static function findUserSubscription($forum_id, $topic_id = 0)
	{
		global $user;
		
		if(!$user) return null;
		$pk = $user->getPrimaryKey();
		
		trace("***************findUserSubscription user id {$user->$pk} forum_id $forum_id topic_id $topic_id", 3);
		$subscriptions = Query::create(ForumSubscription, "WHERE user_id=:user_id AND forum_id=:forum_id AND topic_id=:topic_id")
			->bind(":user_id",  $user->$pk, ":forum_id", $forum_id, ":topic_id", $topic_id)
			->execute();

		return count($subscriptions) ? $subscriptions[0] : null;
	}
	
	static function getUserTopicSubscriptions($forum_id)
	{
		global $user;
		if(!$user) return null;
		$pk = $user->getPrimaryKey();
		
		return Query::create(ForumSubscription, "WHERE user_id=:user_id AND forum_id=:forum_id AND topic_id > 0")
			->bind(":user_id",  $user->$pk, ":forum_id", $forum_id)
			->execute();
	}
	
	function ForumSubscription()
	{
		$this->primary_key = "forum_subscription_id";
		$this->table = "forum_subscription";

		$this->DataItem(func_get_args());
		
	}
}
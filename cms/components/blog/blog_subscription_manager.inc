<?php
Fakoli::using("blog", "email");

class BlogSubscriptionManager
{
	var $daily_digest = array();		///< Array of formatted blog articles new since yesterday indexed by blog_id
	var $weekly_digest = array();		///< Array of formatted blog articles new since last week indexed by blog_id
	var $subscribers;
	var $blogs;
	
	function BlogSubscriptionManager()
	{
		$this->subscribers = Query::create(BlogSubscriber, "WHERE subscription_type IN ('daily', 'weekly') AND blog_id IN (SELECT blog_id FROM blog WHERE published=true)")->execute();
		
		$this->daily_digest = $this->getDailyDigest();
		$this->weekly_digest = $this->getWeeklyDigest();	
	}
	
	static function generateToken()
	{
		return md5(guid());
	}
	
	function getDailyDigest()
	{
		$today = new DateTime(today() . "00:00:00");
		$yesterday = clone($today);
		$yesterday->modify("-1 days");
		
		$articles = Query::create(Article, "WHERE published=true AND publish_date=:yesterday")
			->bind(":yesterday", $yesterday->format("Y-m-d H:i:s"))
			->execute();
	
		if(!count($articles)) return null;
		
		$daily_digest = array();
		foreach($articles as $article)
		{
			$blog = $article->Blog();
		
			if(!$blog || !$blog->published) continue;
				
			if(!array_key_exists($blog->blog_id, $this->blogs))
			{
				$this->blogs[$blog->blog_id] = $blog;
			}			
			$daily_digest[$blog->blog_id][] = $article;
		}
		
		return $daily_digest;
	}
	
	function getWeeklyDigest()
	{
		$today = new DateTime(now());		
		$start_of_week = Settings::getValue("blog", "weekly_digest_day");

		if($today->format("D") != $start_of_week)
		{
			return;
		}
		
		$last_week = clone $today;
		$last_week->modify("-7 days");
	
		$articles = Query::create(Article, "WHERE published=true AND publish_date BETWEEN :last_week AND :today")
			->bind(":last_week", $last_week->format("Y-m-d"), ":today", $today->format("Y-m-d"))
			->execute();
	
		if(!count($articles)) return null;
	
		$weekly_digest = array();
		foreach($articles as $article)
		{
			$blog = $article->Blog();
	
			if(!$blog || !$blog->published) continue;
	
			if(!array_key_exists($blog->blog_id, $this->blogs))
			{
				$this->blogs[$blog->blog_id] = $blog;
			}
			$weekly_digest[$blog->blog_id][] = $article;
		}
		
		return $weekly_digest;
	}
	
	function sendDigests()
	{
		if(!count($this->subscribers)) return;
		
		foreach($this->subscribers as $subscriber)
		{
			$this->sendDigest($subscriber);
		}
	}
	
	function sendDigest($subscriber)
	{
		$blog_id = $subscriber->blog_id;
		$blog = $this->blogs[$blog_id];
			
		if(($subscriber->subscription_type == "daily" && !array_key_exists($blog_id, $this->daily_digest)) ||
			($subscriber->subscription_type == "weekly" && !array_key_exists($blog_id, $this->weekly_digest)))
		{
			return;
		}
		$articles = ($subscriber->subscription_type == "daily") ? $this->daily_digest[$blog_id] : $this->weekly_digest[$blog_id];
	
		$sitename = Settings::getValue("settings", "sitename");
		$label = $subscriber->format("{subscription_type:prettify}");
		
		$subject = "{$sitename} $label Digest for {$blog->title}";
			
		$url = $blog->getURL();
		
		// set pseudo field for use in formatArticleMessage
		if (count($articles))
		{
			foreach($articles as $article)
			{
				$article->set("url", $url);
			}
		}
			
		$message .= formatItems($articles, "{BlogSubscriptionManager::formatArticleMessage}","\n<br/><hr/><br/>\n");
		$message .= $this->formatUnsubscribeMessageFooter($subscriber);
			
		$emailHandler = new EmailHandler($subscriber->subscriber_email, $subject, $message);
		$emailHandler->send();	
	}

	/**
	 * Format the article for inclusion in the email message. Only include author name and
	 * publisher if provided.
	 * 
	 * @param obj $article class Article
	 */
	static function formatArticleMessage($article)
	{
		$authorName = $article->getAuthorName();
	
		if($authorName)
		{
			$author = "<br><label>Author </label>$authorName";
		}
		
		if($article->publication)
		{
			$publisher = " {$article->publication}";
		}

		// pseudo field
		$url = $article->get("url");
		
		return $article->format("<h3>{title}</h3><br>{message}<br><br>{$url}?article_id={article_id}{$author}<br><label>Published </label> {publish_date:short}{$publisher}");
	}
	
	function formatUnsubscribeMessageFooter($subscriber)
	{
		global $config;
						
		$url =  "http://" . $config["http_host"]  . "/blog_subscription_preferences?blog_subscriber_id={blog_subscriber_id}";
		$messageFooter = $subscriber->format("</br></br>To unsubscribe to these messages or change your subscription preferences, go to $url\n\n");
		
		return $messageFooter;
	}
	
	/*
	* Subscribe to entire forum, leave topic_id blank.
	* If user is subscribed to any topics within this forum,
	* delete the subscriptions.
	*/
	static function updateSubscription($subscriber, $subscription_type)
	{
		// update subscription type for either a forum or topic subscription
		if($subscription_type != subscription_unsubscribe AND
				$subscription_type != $subscriber->subscription_type)
		{
			$subscriber->filter = new InclusionFilter("subscription_type");
			$subscriber->subscription_type = $subscription_type;
			$subscriber->save();
		}
		elseif($subscription_type == subscription_unsubscribe)
		{
			$subscriber->delete();
		}
	}
	
	static function formatTitleLink($blogSubscriber)
	{
		$blog = $blogSubscriber->Blog();
		
		if($blog->published)
		{
			$url = $blog->getURL();		
			$out = $blog->format("<a href='$url'>{title:65}</a>");
		}
		else
		{
			$out = $blog->format("{title:65}");
		}
		
		return $out;
	}
	
	static function renderSubscriptionTypeSelect($blogSubscriber)
	{
		$subscriptionTypes = BlogSubscriber::$subscriptionTypes;
		$subscriptionTypes["unsubscribe"] = "Unsubscribe";
		
		if(!Settings::getValue("blog", "allow_subscription_options"))
		{
			unset($subscriptionTypes["daily"]);
			unset($subscriptionTypes["weekly"]);
		}
		
		if(!array_key_exists($blogSubscriber->subscription_type, $subscriptionTypes))
		{
			$blogSubscriber->subscription_type = "instant";
		}
		
		$current = $blogSubscriber->subscription_type;
		$blog_subscriber_id = $blogSubscriber->blog_subscriber_id;
		
		foreach($subscriptionTypes as $value => $name)
		{
			$selected = ($current == $value) ? " checked" : "";
			$field = "blog_subscriber_id_{$blog_subscriber_id}[$value]";
			$html .= "<input style='border: none' type='radio' id='{$field}' name='{$blog_subscriber_id}[]' value='$value' $selected>$name</option><br>\n";
		}
		
		return $html;
	}
	
	static function writeSubscriptionPreferenceScript($div_id)
	{
			ob_start();
			?>
		<script type="text/javascript" src="/components/blog/js/blog_subscriber_preference_manager.js"></script>
		<script type="text/javascript">
		var blogSubscriberMgr;
		
		window.addEvent('domready', function()
		{
			blogSubscriberMgr = new BlogSubscriberPreferenceManager('<?php echo $div_id ?>');
		});
		
		</script>
		<?
			$script .= ob_get_contents();
			ob_end_clean();
				
			return $script;	
	}
}
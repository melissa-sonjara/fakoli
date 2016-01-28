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

class BlogSubscriber extends DataItem
{
	var $table = "blog_subscriber";
	var $primary_key = "blog_subscriber_id";
	
	// Fields
	var $fields = array("blog_subscriber_id"	=>  Number,
						"blog_id"				=>	Number,
						"subscriber_email"		=>  String,
						"first_name"			=>	String,
						"last_name"				=>	String,
						"organization"			=>	String,
						"subscription_type"		=>	String,
						"subscription_token"	=>	String
			);
	
	// Hidden Fields
	var $hiddenFields = array("subscription_token");
	
	// Relations
	
	var $relations = array("Blog"		=>	"Blog"
						   );
	
	// Subscription type options are only available to web sites that set up a cron job
	// and verify this using CMS Application Settings.
	static $subscriptionTypes = array(
			"instant"	=>	"Instant Notification",
			"daily"		=>  "Daily Digest",
			"weekly"	=>	"Weekly Digest",
	);	
			
	/**
	 * Retreive the blog subscriber record from the token
	 * @param string $token the token identifying the subscriber record
	 */
	static function fromToken($token)
	{
		$subscriber = Query::create(BlogSubscriber, "WHERE subscription_token=:s")
						   ->bind(":s", $token)
						   ->executeSingle();
		return $subscriber;
	}
	
	function Blog()
	{
		return $this->getRelated(Blog);
	}
	
	/**
	 * Merge Code 'unsubscribe_url'
	 * 
	 * @return string
	 */
	function getUnsubscribeUrl()
	{
		global $config;
		$url = ($_SERVER['HTTPS']) ? "https://" : "http://";
	
		return $url . $config["http_host"] . "/action/blog/unsubscribe?subscription_token={$this->subscription_token}";
	}	
	
	/**
	 * Merge code 'blog_url'
	 * 
	 * article_id is supplied in the calling code.
	 * 
	 * @return string
	 */
	function getBlogUrl()
	{
		return $this->Blog()->getURL();
	}
}

?>
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
	// Fields
	var $fields = array("blog_subscriber_id"	=>  Number,
						"blog_id"				=>	Number,
						"subscriber_email"		=>  String,
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
	
		return $url . $config["http_host"] . "/action/blog/unsubscribe?blog_subscriber_id={$this->blog_subscriber_id}";
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
		global $config;
		$url = ($_SERVER['HTTPS']) ? "https://" : "http://";
	
		return $url . $config["http_host"].  "/". $this->format("{Blog.identifier}");
	}
						
	function BlogSubscriber()
	{
		$this->primary_key = "blog_subscriber_id";
		$this->table = "blog_subscriber";
		
		$this->DataItem(func_get_args());
	}
}

?>
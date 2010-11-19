<?php
/*
 * Dummy datamodel to build contact us form. No db table
 * - data is not retreived or stored. 
 * 
 * Janice Gallant for Sonjara, Inc.
 * 
 * 9/24/2010
 */

class ContactUs extends DataItem
{
	var $fields = array(
					"contact_us_id"		=>	Number,
					"full_name"			=>	String,
					"email"				=>	String,
					"contact_topic_id"	=>	String,
					"subject"			=>	String,
					"message"			=>	Text
	);
	
	var $relations = array(
				"ContactTopic"	=>	ContactTopic
	);
	
	function ContactTopic()
	{
		return $this->getRelated(ContactTopic);
	}
	
	function ContactUs()
	{
		$this->primary_key = "contact_us_id";
		$this->table = "contact_us";

		$this->DataItem(func_get_args());
	}
	
}


class ContactTopic extends DataItem
{
	var $fields = array(
					"contact_topic_id"	=>	Number,
					"topic"				=>	String,
					"recipients"		=>	Text,
					"sort_order"		=>	Number
	);
	
	var $relations = array(
					"Sites"		=>	Site
	);
	
	static function formatList()
	{
		global $page;
		$topicList = array();
		$siteCount = queryValue(Site, "COUNT(1)", "");
		if($siteCount == 1)
		{		
			$topics = query(ContactTopic, "ORDER BY sort_order");
		}
		else
		{
			$site = $page->Site();
			if(!$site) $site = Site::getSite();
			$topics = query(ContactTopic, "WHERE contact_topic_id IN (SELECT contact_topic_id FROM contact_topic_site_xref WHERE site_id = {$site->site_id}) ORDER BY sort_order");
		}
		if(count($topics) > 0)
		{
			foreach($topics as $topic)
				$topicList[$topic->contact_topic_id] = $topic->topic;
		}
		return $topicList;
	}

	function Sites()
	{
		return $this->crossReference(Site, ContactTopicSiteXref);
	}
	
	function ContactTopic()
	{
		$this->primary_key = "contact_topic_id";
		$this->table = "contact_topic";

		$this->DataItem(func_get_args());
	}
}

class ContactTopicSiteXref extends DataItem
{
	var $fields = array(
			"contact_topic_site_xref_id"	=>	Number,
			"contact_topic_id"				=>	Number,
			"site_id"						=> 	Number
	);
	
	function ContactTopicSiteXref()
	{
		$this->primary_key = "contact_topic_site_xref_id";
		$this->table = "contact_topic_site_xref";

		$this->DataItem(func_get_args());
	}
	
}


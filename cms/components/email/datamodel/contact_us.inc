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
		if($this->contact_topic_id)
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
			if($site)
			{
				$siteTopics = Query::create(ContactTopicSiteXref, "WHERE site_id=:s")
					->bind(":s", $site->site_id)
					->execute();
					
				if(count($siteTopics) > 0)
				{
					$topicKeyList = formatItems($siteTopics, "{contact_topic_id}", ",");	
					$topics = query(ContactTopic, "WHERE contact_topic_id IN ($topicKeyList) ORDER BY sort_order");
				}
			}
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


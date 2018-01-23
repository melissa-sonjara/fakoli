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

class BlogManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "blog";
		$component->description = "Basic blog functionality for Fakoli/CMS";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 2;
		$component->enabled = true;
		
		return $component;
	}
	
	static function getAdminMenu()
	{
		return array("Site Content"	=> array
			(

				"Blogs"	=>	array("page"		=>	"/admin/blogs",
								  "role"		=>	"admin,editor",
								  "weight"	=>	5,
								  "icon"		=>	"/components/blog/images/blog.svg")
			),
					 "Email Management"	=>	array
			(
				"Blog Subscribers"	=>	array("page"	=>	"/admin/blog_subscribers",
											  "role"	=>	"admin",
											  "weight"	=>	10,
											  "icon"	=>	"user-circle")
			)
		);
	}	
	
	var $allow_sessionless_handlers = array("rss", "send_digest", "unsubscribe", "subscribe_dialog"); // Allow anonymous access to the rss action handler
	
	static function getStyles()
	{
		return array("/fakoli/css/blog.css");
	}
	 
	static function getScripts()
	{
		return array("/components/blog/js/blog_manager.js");
	}
	
	static function subscribeToEvents()
	{
		return array("ResolveIdentifier"			=> array(BlogManager, displayBlog),
					 "EnumerateItems"				=> array(BlogManager, enumerateItems),
					 "RegisterBlogViewHandlers"		=> array(BlogView, registerStandardHandlers),
					 "upgradeComponent"				=>	array(BlogManager, upgradeComponent),
					 "RegisterSerializationHandler"	=>	array(BlogManager, registerSerializationHandler),
					 "RegisterTaxonomyClasses"		=>	array(BlogManager, registerTaxonomyClasses),
					 "DeleteUser"					=>	array(BlogManager, deleteUser),
					 "ComponentScanComplete"		=> array(BlogManager, setDefaults),
					 "RegisterSearchables"			=>	array(BlogManager, registerSearchables),
					 "RegisterSolrAdapters"			=>	array(BlogManager, registerSolrAdapter),
					 "RegisterScheduledTaskWorkers"	=>	array(BlogManager, registerScheduledTaskWorkers),
					 "RegisterVersionedContent"		=>	array(BlogManager, registerVersionedContent)
		);
	}
}

?>
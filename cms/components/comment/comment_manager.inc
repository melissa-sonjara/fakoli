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

class CommentAdapter
{
	var $component;
	var $parentClass;
	var $xrefClass;
	var $imported = false;
	
	function __construct($component, $parentClass, $xrefClass)
	{
		$this->component   = $component;
		$this->parentClass = $parentClass;
		$this->xrefClass   = $xrefClass;
	}

	function importComponent()
	{
		if (!$this->imported)
		{
			Fakoli::using($this->component);
			$this->imported = true;
		}
	}
	
	function createParent()
	{
		$this->importComponent();
		$parent = new $this->parentClass;
		if (isset($_GET[$parent->getPrimaryKey()]) && checkNumeric($_GET[$parent->getPrimaryKey()]))
		{
			$parent->load($_GET[$parent->getPrimaryKey()]);
		}
		
		return $parent; 
	}
	
	function createXref()
	{
		$this->importComponent();		
		return new $this->xrefClass;
	}	
}

class CommentManager
{	
	function CommentManager()
	{
		
	}
	
	static function setDefaults()
	{		
		Settings::setDefaultValue("comment", "allow_anonymous_commenting", false, Boolean);
		Settings::setDefaultValue("comment", "publish_automatically", true, Boolean);
	}
	
	static function onInitialize()
	{
		if (!Cache::get("comment_adapters"))
		{
			ComponentManager::fireEvent("RegisterCommentAdapters");
		}
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new CommentUpgradeManager();
		$mgr->upgrade($version);
	}
	
	static function registerAdapter($component, $parentClass, $xrefClass)
	{
		$adapter = new CommentAdapter($component, $parentClass, $xrefClass);
		
		$adapters = CommentManager::getAdapters();
		$proto = $adapter->createParent();
		$adapters[$proto->getPrimaryKey()] = $adapter;
		
		Cache::put("comment_adapters", $adapters);
	}
	
	static function getAdapters()
	{
		$adapters = Cache::get("comment_adapters");
		if (!$adapters) $adapters = array();
		
		return $adapters;
	}
	
	static function getAdapter()
	{
		$adapters = CommentManager::getAdapters();
		
		foreach($_GET as $key => $value)
		{
			if (array_key_exists($key, $adapters))
			{
				return $adapters[$key];
			}
		}
	}
}
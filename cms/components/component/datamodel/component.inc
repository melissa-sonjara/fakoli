<?php
/**\addtogroup component
 * @{
 */


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

require_once realpath(dirname(__FILE__)."/../../../../framework/data_item.inc");

class Component extends DataItem
{
	var $table = "component";
	var $primary_key = "component_id";
	var	$default_format = "{getPrettyName()}";
		
	// Fields
	var $fields = array("component_id"		=>	'Number',
						"name"				=>	'String',
						"version"			=>	'String',
						"author"				=>	'String',
						"description"		=>	'Text',
						"priority"			=>	'Number',
						"enabled"			=>	'Boolean',
						"component_path"		=>	'String',
						"last_modified"		=>	'Timestamp');
	
	// Relations
	var $relations = array("ComponentPages"	=>	'ComponentPage');
	
	function ComponentPages()
	{
		$pages = Query::create(ComponentPage, "WHERE component=:i ORDER BY identifier")
					  ->bind(":i", $this->name)
					  ->execute();
					
		return $pages;
	}
	
	function getPrettyName()
	{
		return prettify($this->name);
	}
	
	static function formatOptions()
	{
		$options = array();
		
		$components = removeDuplicates(Query::create(Component, "ORDER BY name")->execute(), "name");
		if(count($components) > 0)
		{
			foreach($components as $component)
			{
				$options[$component->name] = $component->name;
			}
		}
		
		return $options;
	}
	
	static function getComponent($name)
	{
		return Query::create(Component, "WHERE name=:n")->bind(":n", $name)->executeSingle();
	}
	
	function loadManifest()
	{
		Fakoli::using($this->name);
		$cl = Fakoli::getComponentClassRoot($this->name)."Manifest";
		if (class_exists($cl))
		{
			return new $cl;
		}		
		else 
		{
			return null;
		}
	}
}			

/**@}*/?>

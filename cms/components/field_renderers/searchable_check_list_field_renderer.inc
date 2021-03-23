<?php
/**\addtogroup field_renderers
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

Fakoli::usingFeature("field_renderers");
Fakoli::using("component", "search");

class SearchableCheckListFieldRenderer extends CheckListFieldRenderer
{
	function SearchableCheckListFieldRenderer($form, $field, $label)
	{
				
		$options = array();
		SearchManager::buildSearchMap();
		if(count(SearchManager::$map)>0)
		{
			foreach(SearchManager::$map as $component => $searchables)
			{	
				Fakoli::using($component);
				
				if(count($searchables) > 0 )
				{
					foreach($searchables as $searchable)
					{
						$obj = new $searchable;
						$name = $obj->prettifyClassName(true);
						
						$options[$searchable] = $name;
					}
				}
			}
		}
		
		$this->CheckListFieldRenderer($form, $field, $label, $options, false);		
	}

	/**
	 * To modify the displayed label for a searchable class name, send
	 * parameter string in the format
	 * class_name, myLabel, class_name2, myLabel2...
	 * e.g.
	 * "Article", "News" will change the option label form Articles to News
	 */
	function alias()
	{
		for($i = 0; $i < func_num_args(); $i += 2)
		{
			$class_name = func_get_arg($i);
			$label = func_get_arg($i + 1);
			if(array_key_exists($class_name, $this->options))
			{
				$this->options[$class_name] = $label;			
			}
		}
	}

	function renderSearchField($field)
	{
		if (!$this->parent->params->get($field, ""))
		{
			$this->parent->params->set($field, "", implode(",", array_keys($this->options)));
		}
		
		trace("Options: ".$this->parent->params->get($field), 3);
		
		return parent::renderSearchField($field);
	}
}
		
/**@}*/?>
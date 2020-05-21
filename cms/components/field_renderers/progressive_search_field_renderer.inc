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

Fakoli::usingFeature("field_renderers");


class ProgressiveSearchFieldRenderer extends StringFieldRenderer
{
	var $handler = "";				///< path to the action handler that retrieves and displays results
	var $minimumLength = 4;			///< minimum number of characters user must enter before search is called
	var $cssClass = "scrollbox";	///< class for the progressive search results
	var $scrollBoxWidth = "300px";	///< width of progressive search results
	var $size = 20;					///< size of the input field for entering search string
	
	var $browse = false;
	var $browseLabel = "Browse";
	var $browseCss = "button";
	
	/**
	 * 
	 * 
	 * @param $parent
	 * @param $field
	 * @param $label
	 * @param $handler - the path to the action handler that retrieves the results
	 * and displays them in the scrollbox (e.g., '/action/task/task_search_handler'
	 */
	function ProgressiveSearchFieldRenderer(&$parent, $field, $label = "", $handler)
	{
		$this->FieldRenderer($parent);
		if ($field && !$parent->data->hasField($field))
		{
			$parent->add($this, $field);
			$parent->overrides[$field]['label'] = $label;
		}
		else if($label)
		{
			$parent->override($field, $label, $this);	
		}
		
		$this->handler = $handler;
		$this->autocomplete = false;
	}
	
	function allowBrowse($text = "", $css = "")
	{
		$this->browse = true;
		if ($text) $this->browseText = jsSafe($text);
		if ($css) $this->browseCss = jsSafe($css);
		return $this;
	}
	
	function renderScript($field)
	{	
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)
			|| array_key_exists($field, $this->parent->hidden)) return "";
		
		$searchOptions = "{'search': '{$this->handler}', minimumLength: {$this->minimumLength}, 'cssClass': '{$this->cssClass}', 'width': '{$this->scrollBoxWidth}', 'parameter': '{$field}', 'browse': {$browse}, 'browseText': '{$this->browseText}', 'browseCssClass': '{$this->browseCss}'";
		
		echo "<script type='text/javascript'>\n";
		echo "var {$field}_progressiveSearch;\n";
		echo "window.addEvent('domready', function()\n";
		echo "{\n";
		echo "\t{$field}_progressiveSearch = new ProgressiveSearch('{$this->parent->id}_{$field}', {$searchOptions});\n";
		echo "});\n";
		echo "</script>\n";
	}
}
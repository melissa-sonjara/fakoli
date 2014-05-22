<?php
/**************************************************************

Copyright (c) 2013 Sonjara, Inc

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

Fakoli::usingFeature("facet_manager");

class SearchResultTypeFacetFilter extends AbstractFacetFilter
{
	var $taxonomy;
	
	var $width = "200px";
	var $height = "100px";
	var $dropdown = true;
	var $dropdownMaxWidth = "200px";
	var $dropdownMessage = "Click to Select Types";
	var $classes = array();
	
	function __construct($items)
	{
		foreach($items as $item)
		{
			$className = $item->prettifyClassName(true);
			$this->classes[$className] = true;
		}
	}

	function getID()
	{
		return "search_result_type";
	}
	
	function onTagItem($item, $attrs)
	{
		// Need to work with underlying object if passed is a search result container
		$className = $item->prettifyClassName(true);
		
		$attrs["data-search_result_type"] = $className;

		return $attrs;
	}	

	function writeScript()
	{
		ob_start();

		if ($this->dropdown)
		{
?>
	var search_result_type_select =  new MultiSelect('search_result_type_container', {maxWidth: '<?echo $this->dropdownMaxWidth?>', message: '<?echo jsSafe($this->dropdownMessage)?>'});
	var search_result_type_handler = new MultiSelectFacetHandler('search_result_type', search_result_type_select, <?echo $this->manager->target->getID()?>_facet_manager);
<?
		}
		
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}
	
	function drawFacet()
	{
		$types = array_keys($this->classes);
			
		if ($this->width && $this->height)
		{
			$resizable = (!$this->dropdown && $this->resizable) ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='search_result_type_container' class='scrollbox{$resizable}{$dropdown}' style='width: {$this->width}; height: {$this->height}; overflow: auto'>\n";
		}
									
		if ($this->width && $this->height)
		{
			echo "</div>\n";
		}

	}
	
	function renderCheckboxes($items)
	{
		foreach($items as $item)
		{
			echo "<input type='checkbox' class='checkbox' name='search_result_types[$item]' id='checkbox_search_result_types_$item' value='$item' checked='checked'>&nbsp;";
			echo "<label for='checkbox_search_result_types_$item'>".$item."</label>";
			echo "<br/>";
		}
	}
}
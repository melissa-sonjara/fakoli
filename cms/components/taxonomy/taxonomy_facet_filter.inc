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

class TaxonomyFacetFilter extends AbstractFacetFilter
{
	var $taxonomy;
	
	var $width = "200px";
	var $height = "100px";
	var $dropdown = true;
	var $dropdownMaxWidth = "200px";
	var $dropdownMessage = "Click to Select Terms";
	
	function __construct($taxonomy)
	{
		$this->taxonomy = $taxonomy;
	}

	function getID()
	{
		return $this->taxonomy->identifier;
	}
	
	function onTagItem($item, $attrs)
	{
		$term_ids = $this->taxonomy->getAssociatedTermIds($item);
		if ($term_ids)
		{
			$attrs["data-taxonomy-{$this->taxonomy->identifier}"] = $term_ids;
		}

		return $attrs;
	}	

	function writeScript()
	{
		ob_start();

		if ($this->dropdown)
		{
?>
	var <?echo $this->taxonomy->identifier?>_select =  new MultiSelect('<?echo $this->taxonomy->identifier?>_container', {maxWidth: '<?echo $this->dropdownMaxWidth?>', message: '<?echo jsSafe($this->dropdownMessage)?>'});
	var <?echo $this->taxonomy->identifier?>_handler = new TaxonomyFacetHandler('<?echo $this->taxonomy->identifier?>', <?echo $this->taxonomy->identifier?>_select, <?echo $this->manager->target->getID()?>_facet_manager);
<?
		}
		
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}
	
	function drawFacet()
	{
		$items = $this->taxonomy->Terms("ORDER BY term");
		
		if (count($items) == 0) return;
		
		if ($this->width && $this->height)
		{
			$resizable = $this->resizable ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='{$this->taxonomy->identifier}_container' class='scrollbox{$resizable}{$dropdown}' style='width: {$this->width}; height: {$this->height}; overflow: auto'>\n";
		}
									
		$this->renderCheckBoxes($items);  

		if ($this->width && $this->height)
		{
			echo "</div>\n";
		}

	}
	
	function renderCheckboxes($items)
	{
		foreach($items as $item)
		{
			$p = $item->term_id;
			
			echo "<input type='checkbox' class='checkbox' name='{$this->taxonomy->identifier}[$p]' id='checkbox_{$this->taxonomy->identifier}_$p' value='$p'";
			if (array_key_exists($p, $selected))
			{
				echo " checked";
			}
			echo ">&nbsp;";
			echo "<label for='checkbox_{$this->taxonomy->identifier}_$p'>".$item->term."</label>";
			echo "<br/>";
		}
	}
}
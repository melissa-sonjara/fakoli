<?php
/**\addtogroup datadisplay
 * @{
 */
/**************************************************************

Copyright (c) 2007-2013 Sonjara, Inc

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

/**
 * Interface for objects that can be filtered by FacetManager facet filters.
 * Any object that is to be filtered by a FacetManager must implement this interface.
 */
interface FacetFilterable
{
	/**
	 * @return string the unique ID of the filterable control
	 */
	public function getID();

	/**
	 * Registers a handler function to be called when an item is tagged for filtering. 
	 * The handler function should take two parameters: the item being tagged and an 
	 * associative array of attributes to be applied to the item in the output. 
	 * The handler should return the modified attributes array.
	 * @param callable $handler a function or method to handle item tagging for facet filtering
	 */
	public function addFacetTaggingHandler($handler);
}

/**
 * AbstractFacetFilter is the base class for facet filters that can be added to a FacetManager 
 * and applied to filterable controls such as DataListView.
 */
abstract class AbstractFacetFilter
{
	/**
	 * Method that is called to tag an item for filtering. This method should be implemented 
	 * to add data attributes to the item
	 * @param DataItem $item the item being tagged
	 * @param array $attrs the existing attributes for the item, as an associative array of
	 * attribute name => value pairs.
	 * @return array the modified attributes for the item.
	 */
	abstract function onTagItem($item, $attrs);

	/**
	 * Method that is called to write the client-side script required to implement this facet filter. 
	 * This method should return a string containing the Javascript code required to implement the 
	 * facet filter on the client side.
	 * 
	 * @return string the Javascript code required to implement the facet filter on the client side.
	 */
	abstract function writeScript();

	/**
	 * Method that is called to draw the user interface for this facet filter. Method should
	 * output the HTML for the facet filter UI.
	 * @return void
	 */
	abstract function drawFacet();

	/**
	 * Server-side method that is called to filter an item based on the current state of the facet filter. 
	 * This method should return true if the item should be included in the filtered results, or false if 
	 * it should be excluded.
	 * @param DataItem $item the item to be filtered
	 * @return boolean whether the item should be included in the filtered results
	 */
	abstract function filterItem($item);
}

/**
 * The FacetManager class provides the interface for managing facet filters and applying them to 
 * filterable controls such as DataListView.
 */
class FacetManager
{
	var $id;
	var $target;
	var $facets;

	/**
	 * Constructor. Creates a new FacetManager for the given target control.
	 * @param string $id the client-side ID for this FacetManager
	 * @param FacetFilterable $target the control to which this FacetManager applies
	 */
	function __construct($id, $target)
	{
		$this->id = $id;
		$this->target = $target;
		$this->facets = array();
		
		$this->target->addFacetTaggingHandler(array($this, 'onTagItem'));

		if (method_exists($target, 'addFilterRowHandler'))
		{
			$this->target->addFilterRowHandler(array($this, 'onFilterItem'));
		}
	}
	
	/**
	 * Adds a facet filter to this FacetManager with the given title. 
	 * The facet filter should be an instance of a class that extends AbstractFacetFilter.
	 * 
	 * @param string $title the title for this facet filter, to be displayed in the UI
	 * @param AbstractFacetFilter $facet the facet filter to be added
	 */
	function addFacet($title, $facet)
	{
		$this->facets[$title] = $facet;
		$facet->manager = $this;
		
		return $facet;
	}
	
	/**
	 * Callback function that is called when an item is tagged for filtering. This function applies all facet filters
	 * to the item and returns the modified attributes for the item.
	 * @param DataItem $item the item being tagged
	 * @param array $attrs the existing attributes for the item, as an associative array of
	 * attribute name => value pairs.
	 * @return array the modified attributes for the item after applying all facet filters.
	 * @see AbstractFacetFilter::onTagItem()
	 */
	function onTagItem($item, $attrs)
	{
		if ($item instanceof AbstractSearchResult)
		{
			$item = $item->item;
		}
		
		foreach ($this->facets as $title => $facet)
		{
			$attrs = $facet->onTagItem($item, $attrs);
		}
		
		return $attrs;
	}
	
	/**
	 * Callback function that is called to filter an item based on the current state of the facet filters. 
	 * This function applies all facet filters to the item and returns true if the item should be included 
	 * in the filtered results, or false if it should be excluded.
	 * @param DataItem $item the item to be filtered
	 * @return boolean whether the item should be included in the filtered results after applying all facet filters.
	 * @see AbstractFacetFilter::filterItem()
	 */
	function onFilterItem($item)
	{
		if ($item instanceof AbstractSearchResult)
		{
			$item = $item->item;
		}
		
		foreach ($this->facets as $title => $facet)
		{
			$show = $facet->filterItem($item);
			if (!$show)
			{
				return false;
			}
		}
		
		return true;
	}

	/**
	 * Writes the client-side script required to implement this FacetManager and its facet filters. 
	 * @return string the Javascript code required to implement the FacetManager and its facet filters on the client side.
	 */
	function writeScript()
	{
		$script = "<script type=\"text/javascript\">\nwindow.addEvent('domready', function()\n{\n\tvar {$this->target->getID()}_facet_manager = new FacetManager('{$this->target->getID()}');\n";
		
		foreach($this->facets as $facet)
		{
			$script .= $facet->writeScript();
		}
		
		$script .= "\n});\n</script>";
		return $script;
	}

	/**
	 * Draws the user interface for this FacetManager and its facet filters. This method outputs the HTML
	 * for the facet filter UI.
	 * @return void
	 */
	function drawForm()
	{
?>
<div id='<?echo $this->id?>' class='facet_form'>
<?
		foreach($this->facets as $title => $facet)
		{
?>
<span style='display: inline-block'><label for='<?echo $facet->getID()?>'><?echo $title?></label> 
<?
			$facet->drawFacet();
?>
</span>
<?
		}
?>		
</div>	
<?
	}

	/**
	 * Returns the current value of a facet filter based on the request parameters. 
	 * This method is used by facet filters to get the current filter value when filtering items on the server side.
	 */
	function getFacetValue($facetId)
	{
		return $_REQUEST[$facetId] ?: "";
	}
}

/**
 * Variant FacetManager that is designed to be used with a reloadable panel that provides server-side
 * filtering of results. This FacetManager does not apply filters to items directly, but instead provides an interface
 * for generating a query string based on the current state of the facet filters, which can be used to reload the panel 
 * with filtered results from the server.
 */
class PanelFacetManager
{
	var $id;
	var $target;
	var $facets;
	
	/**
	 * Constructor. Creates a new PanelFacetManager for the given target panel.
	 * @param Panel $target the panel to which this PanelFacetManager applies
	 */
	function __construct($target)
	{
		$this->id = $target->getID()."_facet_manager";
		$this->target = $target;
		$this->facets = array();
	}
	
	/**
	 * Adds a facet filter to this PanelFacetManager with the given title. 
	 * The facet filter should be an instance of a class that extends AbstractFacetFilter.
	 * 
	 * @param string $title the title for this facet filter, to be displayed in the UI
	 * @param AbstractFacetFilter $facet the facet filter to be added
	 */
	function addFacet($title, $facet)
	{
		$this->facets[$title] = $facet;
		$facet->manager = $this;
		
		return $facet;
	}	

	/**
	 * Generates the client-side script to configure a FacetManager for dynamic reloading of a target panel
	 * based on the current state of the facet filters. This method should be called in the context of a page 
	 * that contains the target panel, and the generated script will set up event handlers to reload the panel 
	 * with the appropriate query string when facet filters are changed.
	 * @return string the Javascript code required to implement the dynamic reloading of the target panel
	 */
	function writeScript()
	{
		$mgr = $this->target->getID()."_facet_manager";
		$panel = $this->target->getID();
		
		$script = <<<ENDSCRIPT
<script type="text/javascript">
	window.addEvent('domready', function()
	{
		var {$mgr} = new FacetManager(null, 
		{
			onFilterChanged: function()
			{
				document.id('{$panel}').loadPanel('{$this->target->handler}?' + {$mgr}.getQueryString());
			}
		});
		
ENDSCRIPT;
		
		foreach($this->facets as $facet)
		{
			$script .= $facet->writeScript();
		}
		
		$script .= "\n});\n</script>";
		return $script;
	}
	
	/**
	 * Draws the user interface for this PanelFacetManager and its facet filters. This method outputs the HTML
	 * for the facet filter UI.
	 */
	function drawForm()
	{
?>
<div id='<?echo $this->id?>' class='facet_form'>
<?
		foreach($this->facets as $title => $facet)
		{
?>
<span style='display: inline-block'><label for='<?echo $facet->getID()?>'><?echo $title?></label>&nbsp;
<?
			$facet->drawFacet();
?>
</span>
<?
		}
?>		
</div>	
<?
	}
}

/**
 * Basic field filter facet. This facet applies a simple string filter to a specified field of the items being filtered.
 */
class FieldFacetFilter extends AbstractFacetFilter
{
	var $field;
	
	/**
	 * Constructor. Creates a new FieldFacetFilter for the given field.
	 * @param string $field the field to which this facet filter applies
	 */
	function __construct($field)
	{
		$this->field = $field;
	}	

	/**
	 * Returns the ID of this facet filter.
	 * @return string the ID of this facet filter
	 */
	function getID()
	{
		return $this->field;
	}
	
	function onTagItem($item, $attrs)
	{
		if ($item->hasField($this->field)) 
		{
			$attrs["data-{$this->field}"] = strtolower($item->format("{".$this->field.":xml}"));
		}

		return $attrs;
	}

	function writeScript()
	{
		ob_start();
?>
		var <?echo $this->getID()?>_handler = new StringFacetHandler('<?echo $this->getID()?>', '<?echo $this->getID()?>_facet', <?echo $this->manager->target->getID()?>_facet_manager);
<?
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}

	function drawFacet()
	{
?>
<input type='text' name='<?echo $this->field?>' id='<?echo $this->getID()?>_facet' value='' autocomplete='off'/>
<?		
	}

	function filterItem($item)
	{
		$value = $item->get($this->field);
		$filterValue = $this->manager->getFacetValue($this->field);
		
		if (!$filterValue) return true;
		
		return stripos($value, $filterValue) !== false;
	}
}

class ExpressionFacetFilter extends AbstractFacetFilter
{
	var $field;
	var $expression;
	
	function __construct($field, $expression)
	{
		$this->field = $field;
		$this->expression = $expression;
	}

	function getID()
	{
		return $this->field;
	}

	function onTagItem($item, $attrs)
	{
		$attrs["data-{$this->field}"] = strtolower($item->format($this->expression));

		return $attrs;
	}

	function writeScript()
	{
		ob_start();
?>
		var <?echo $this->getID()?>_handler = new StringFacetHandler('<?echo $this->getID()?>', '<?echo $this->getID()?>_facet', <?echo $this->manager->target->getID()?>_facet_manager);
<?
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}

	function drawFacet()
	{
		?>
<input type='text' name='<?echo $this->field?>' id='<?echo $this->getID()?>_facet' value='' autocomplete='off'/>
<?		
	}

	function filterItem($item)
	{
		$value = $item->get($this->field);
		$filterValue = $this->manager->getFacetValue($this->field);
		
		if (!$filterValue) return true;
		
		return stripos($value, $filterValue) !== false;
	}
}

class MultiSelectFacetFilter extends AbstractFacetFilter
{
	var $formatAdapters;
	var $id;
	
	var $width = "200px";
	var $height = "100px";
	var $dropdown = true;
	var $dropdownMaxWidth = "200px";
	var $dropdownMessage = "Click to Select Terms";
	
	function __construct($id, $formatAdapters)
	{
		$this->id = $id;
		$this->formatAdapters = $formatAdapters;	
	}

	function getID()
	{
		return $this->id;
	}
	
	function onTagItem($item, $attrs)
	{
		$cl = get_class($item);
		
		if (array_key_exists($cl, $this->formatAdapters))
		{
			$value = $item->format($this->formatAdapters[$cl]);
		} 
			
		if ($value)
		{
			$attrs["data-{$this->id}"] = $value;
		}
		
		return $attrs;
	}
	
	function writeScript()
	{
		ob_start();
		
		if ($this->dropdown)
		{
			?>
			var <?echo $this->id?>_select =  new MultiSelect('<?echo $this->id?>_container', {maxWidth: '<?echo $this->dropdownMaxWidth?>', message: '<?echo jsSafe($this->dropdownMessage)?>'});
			var <?echo $this->id?>_handler = new MultiSelectFacetHandler('<?echo $this->id?>', <?echo $this->id?>_select, <?echo $this->manager->target->getID()?>_facet_manager);
		<?
		}
		
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
		
	}
	
	function drawFacet()
	{
		if ($this->width && $this->height)
		{
			$resizable = (!$this->dropdown && $this->resizable) ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='{$this->id}_container' class='scrollbox{$resizable}{$dropdown}' style='width: {$this->width}; height: {$this->height}; overflow: auto'></div>\n";
		}		
	}	

	function filterItem($item)
	{
		$cl = get_class($item);
		
		if (array_key_exists($cl, $this->formatAdapters))
		{
			$value = $item->format($this->formatAdapters[$cl]);
		} 
		else
		{
			$value = $item->get($this->field);
		}
		$filterValue = $this->manager->getFacetValue($this->field);
		
		if (!$filterValue) return true;
		
		return $value == $filterValue;
	}
}

class CrossReferenceFacetFilter extends AbstractFacetFilter
{
	var $id;
	var $format;
	var $xrefAdapters;
	var $related;
	
	var $width = "200px";
	var $height = "100px";
	var $dropdown = true;
	var $dropdownMaxWidth = "200px";
	var $dropdownMessage = "Click to Select Terms";
	
	function __construct($id, $xrefAdapters, $format)
	{
		$this->id = $id;
		$this->xrefAdapters = $xrefAdapters;
		$this->format = $format;
		$this->related = array();
	}
	
	function getID()
	{
		return $this->id;
	}
	
	function onTagItem($item, $attrs)
	{
		$cl = get_class($item);
		
		if (array_key_exists($cl, $this->xrefAdapters))
		{
			$adapter = $this->xrefAdapters[$cl];
			$related = $adapter($item);
			if (!is_array($related)) $related = array($related);
			$value = formatItems($related, "{getPrimaryKeyValue()}", ",");
			foreach($related as $r)
			{
				if ($r == null) continue;
				$this->related[$r->getPrimaryKeyValue()] = $r;
			}
		}
		
		if ($value)
		{
			$attrs["data-{$this->id}"] = $value;
		}
		
		return $attrs;
	}
	
	function writeScript()
	{
		ob_start();
		
		if ($this->dropdown)
		{
			?>
			var <?echo $this->id?>_select =  new MultiSelect('<?echo $this->id?>_container', {maxWidth: '<?echo $this->dropdownMaxWidth?>', message: '<?echo jsSafe($this->dropdownMessage)?>'});
			var <?echo $this->id?>_handler = new CrossReferenceFacetHandler('<?echo $this->id?>', <?echo $this->id?>_select, <?echo $this->manager->target->getID()?>_facet_manager);
		<?
		}
		
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
		
	}
	
	function renderCheckboxes()
	{
		$options = array();
		foreach($this->related as $pk => $r)
		{
			$options[$pk] = $r->format($this->format);
		}
		
		asort($options);
		
		foreach($options as $id => $option)
		{
			echo "<label for='checkbox_{$this->id}_{$id}'><input type='checkbox' class='checkbox' name='checkbox_{$this->id}_{$id}' id='checkbox_{$this->id}_{$id}' value='{$id}'";
			echo ">".$option."</label>";
		}
	}
	
	function drawFacet()
	{
		if ($this->width && $this->height)
		{
			$resizable = (!$this->dropdown && $this->resizable) ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='{$this->id}_container' class='scrollbox{$resizable}{$dropdown}' style='width: {$this->width}; height: {$this->height}; overflow: auto'>";
			echo $this->renderCheckboxes();
			echo"</div>\n";
		}		
	}	

	function filterItem($item)
	{
		/*$cl = get_class($item);
		
		if (array_key_exists($cl, $this->xrefAdapters))
		{
			$adapter = $this->xrefAdapters[$cl];
			$related = $adapter($item);
			if (!is_array($related)) $related = array($related);
			$value = formatItems($related, "{getPrimaryKeyValue()}", ",");
		} 
			
		$filterValue = $this->manager->getFacetValue($this->id);
		
		if (!$filterValue) return true;
		
		return stripos($value, $filterValue) !== false;*/

		//TODO: implement server-side filtering for cross reference facet filter
		return true;
	}
}

class CheckListFacetFilter extends AbstractFacetFilter
{
	var $id;
	var $options;
	var $related;
	
	var $width = "200px";
	var $height = "100px";
	var $dropdown = true;
	var $dropdownMaxWidth = "200px";
	var $dropdownMessage = "Click to Select Terms";
	
	function __construct($id, $options)
	{
		$this->id = $id;
		$this->options = $options;
		$this->related = array();
	}
	
	function getID()
	{
		return $this->id;
	}
	
	function onTagItem($item, $attrs)
	{
		$cl = get_class($item);
		
		$value = $item->get($this->id);
		if ($value)
		{
			$attrs["data-{$this->id}"] = $value;
		}
		
		return $attrs;
	}
	
	function writeScript()
	{
		ob_start();
		
		if ($this->dropdown)
		{
			?>
			var <?echo $this->id?>_select =  new MultiSelect('<?echo $this->id?>_container', {maxWidth: '<?echo $this->dropdownMaxWidth?>', message: '<?echo jsSafe($this->dropdownMessage)?>'});
			var <?echo $this->id?>_handler = new CheckListFacetHandler('<?echo $this->id?>', <?echo $this->id?>_select, <?echo $this->manager->target->getID()?>_facet_manager);
		<?
		}
		
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
		
	}
	
	function renderCheckboxes()
	{
		foreach($this->options as $id => $option)
		{
			echo "<label for='checkbox_{$this->id}_{$id}'><input type='checkbox' class='checkbox' name='checkbox_{$this->id}_{$id}' id='checkbox_{$this->id}_{$id}' value='{$id}'";
			echo ">".$option."</label>";
		}
	}
	
	function drawFacet()
	{
		if ($this->width && $this->height)
		{
			$resizable = (!$this->dropdown && $this->resizable) ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='{$this->id}_container' class='scrollbox{$resizable}{$dropdown}' style='width: {$this->width}; height: {$this->height}; overflow: auto'>";
			echo $this->renderCheckboxes();
			echo"</div>\n";
		}		
	}	

	function filterItem($item)
	{
		$value = $item->get($this->id);
		$filterValue = $this->manager->getFacetValue($this->id);
		
		if (!$filterValue) return true;
		
		// Check if value is in the comma-separated list of selected values in filterValue
		$selectedValues = explode(",", $filterValue);
		return in_array($value, $selectedValues);
	}
}

/**@}*/

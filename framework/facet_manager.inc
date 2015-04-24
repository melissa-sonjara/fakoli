<?php
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

interface FacetFilterable
{
	public function getID();
	public function addFacetTaggingHandler($handler);
}

abstract class AbstractFacetFilter
{
	abstract function onTagItem($item, $attrs);
	abstract function writeScript();
	abstract function drawFacet();
}

class FacetManager
{
	var $id;
	var $target;
	var $facets;
	
	function __construct($id, $target)
	{
		$this->id = $id;
		$this->target = $target;
		$this->facets = array();
		
		$this->target->addFacetTaggingHandler(array($this, onTagItem));
	}
	
	function addFacet($title, $facet)
	{
		$this->facets[$title] = $facet;
		$facet->manager = $this;
		
		return $facet;
	}
	
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

class PanelFacetManager
{
	var $id;
	var $target;
	var $facets;
	
	function __construct($id, $target)
	{
		$this->id = $id;
		$this->target = $target;
		$this->facets = array();
	}
	
	function addFacet($title, $facet)
	{
		$this->facets[$title] = $facet;
		$facet->manager = $this;
		
		return $facet;
	}	

	function writeScript()
	{
		$mgr = $this->target->getID()."_facet_manager";
		
		$script = <<<ENDSCRIPT
<script type="text/javascript">
	window.addEvent('domready', function()
	{
		var {$mgr}_facet_manager = new FacetManager();
		{$mgr}_facet_manager.options.onFilterChanged = function()
		{
			document.id('{$mgr}').loadPanel('{$this->panel->handler}?' + this.getQueryString());
		}.bind({$mgr}_facet_manager);
		
ENDSCRIPT;
		
		foreach($this->facets as $facet)
		{
			$script .= $facet->writeScript();
		}
		
		$script .= "\n});\n</script>";
		return $script;
	}
	
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

class FieldFacetFilter extends AbstractFacetFilter
{
	var $field;
	
	function __construct($field)
	{
		$this->field = $field;
	}	

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
}

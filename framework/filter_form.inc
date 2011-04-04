<?php
/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

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

Fakoli::usingFile("framework/search_form.inc");
Fakoli::usingFile("framework/field_renderers/select_field_renderer.inc");

class FilterForm extends SearchForm
{
	var $search;
	
	function FilterForm($target, $method="GET", $action="", $id="")
	{
		global $auto_form_defaults;

		foreach($auto_form_defaults as $field => $value)
		{
			$this->$field = $value;
		}

				
		$this->data =& $target;
		$this->method = $method;
		$this->action = $action;
		$this->id = ($id != "") ? $id : get_class($this->data)."_form";
	
		$this->fields = $this->data->getFields();
		
		$filter = $this->data->getFilter();
		
		// Add in a default renderer for each field
		foreach($this->fields as $field => $type)
		{
			if ($filter && $filter->isExcluded($field)) continue;
			
			$this->renderers[$field] = $this->createRenderer($field);
		}
		
		$this->setFilterParameters();
	
	}
	
	function drawForm()
	{
		$obj =& $this->data;
		$pk = $obj->getPrimaryKey();

		echo "<form id='{$this->id}' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data'>\n";
		echo "<div id=\"filter\">\n";
 
		foreach(array_keys($this->hidden) as $hidden)
		{
			echo "<input id='{$this->id}_{$hidden}' type='hidden' name='$hidden' value='".htmlSafe($obj->get($hidden))."'/>\n";
		}
		
		$this->renderFields($obj);
		$this->renderAdditionalFields($obj);	
	
		echo "</div>\n";
		echo "</form><br><br>\n";
	}
	

	/*
	 * Default for all parameters included in filter is equal
	 * Additional search parameters can be added by calling script with
	 * the syntax:
	 * $filterForm->search->setParam("round_key", "member", implode(",", array_keys($rounds)));
	 * Do not include these additional parameters in the filter.
	 */
	function setFilterParameters()
	{
		$obj = $this->data;
		
		$this->search = new SearchParameters($obj);

		foreach(array_keys($obj->fields) as $field)
		{
			if (!array_key_exists($field, $this->hidden) &&
				!($obj->filter && $obj->filter->isExcluded($field)))
			{
				if(is_numeric($obj->$field))
					$this->search->setParam($field, "equal", $obj->$field);
			}
		}
	}
	
	function getConstraint()
	{
		$constraint = $this->search->generateConstraint(true);
		return $constraint;
	}	
}



class FilterFieldRenderer extends SelectFieldRenderer
{
	var $options;
	var $label;
	var $all = true;
	
	function FilterFieldRenderer(&$form, $field, $label = "", $options)
	{
		$this->options = $options;
		$this->label = ($label) ? $label : prettify($field);
		$this->FieldRenderer($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
		}
	}
	
	
	function renderField($field)
	{
	   	$label = $this->label;
		
		?>
  		<div style="float: left">
		<label for='<?php echo $field?>' class="<?php echo $this->parent->labelCSS ?>"><?php echo $label ?></label>		
    	<select name="<?php echo $field ?>" onchange="this.form.submit();">
    	<?php 
    	if($this->all)
    	{
    	?>
		<option value="">All</option>  
		<?
    	}

     	$fieldValue = $this->parent->data->get($field);
    	trace("Filter form renderer field $fieldValue", 3);
    	foreach($this->options as $value => $name)
		{
			$selected = (!strcmp($value, $fieldValue)) ? " selected" : "";
			echo "<option value='{$value}' $selected>$name</option>\n";
		}
		?>
   		</select>&nbsp;</div>
  		<?		
		
	}
	
	
} // end class FilterFieldRenderer

?>
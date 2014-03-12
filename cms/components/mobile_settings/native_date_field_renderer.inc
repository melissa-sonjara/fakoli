<?php
Fakoli::usingFeature("field_renderers");

class NativeDateFieldRenderer extends DateFieldRenderer
{
	function NativeDateFieldRenderer(&$parent)
	{
		trace("Instantiating NativeDateFieldRenderer", 1);
		$this->DateFieldRenderer($parent);
	}
	
	function renderScript($field)
	{
	}

	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
	
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}
	
	}

	function renderField($field)
	{
		$obj = $this->parent->data;
		$value = $obj->get($field);
		
		// JDG 10/1/2010 check for empty saved string
		$date = "";
		
		if ($value AND $value != '0000-00-00')
		{
			$date = DateTimeTypeRenderer::format($obj->reformatFromSQLDate($value), "Y-m-d");
		}
		
		$this->_startField($field);
		echo "<input id='{$this->parent->id}_{$field}' type='date' name='{$field}_date' value='$date'/>";
		$this->_endField($field);
	}
	
	function renderSearchScript($field, $mode)
	{
	}
	
	function renderSearchField($field, $mode)
	{
		if ($mode == "range")
		{
			$date_from = $this->parent->params->get($field, "from");
			$date_to = $this->parent->params->get($field, "to");
			
			$this->_startField($field);
			
			echo "<input type='date' name='$field:from' value='$date_from'/>";
			echo " to <input type='date' name='$field:to' value='$date_to'/>\n";
			
			$this->_endField($field);
		}
		else
		{
			$date = $this->parent->params->get($field, $mode);
			
			$this->_startField($field);
			echo "<input type='date' name='$field:$mode' value='$date'/>";
			$this->_endField($field);
		}
	}
	
	function renderReadOnly($field)
	{				
		$this->_startField($field);
		echo $this->format($field);
		
		// AJG - if the field is read-only but the form is not, add a hidden field to
		// retain the field value upon submission.
		
		if (!$this->parent->readOnlyForm)
		{
			$obj = $this->parent->data;
			$value = $obj->get($field);
			echo "<input type='hidden' name='$field' value='{$value}'/>";
		}

		$this->_endField();
	}
	
	function preProcess($field)
	{		
		//TODO
	}
}
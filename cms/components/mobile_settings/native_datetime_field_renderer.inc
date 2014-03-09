<?php
Fakoli::usingFeature("field_renderers");

class NativeDateTimeFieldRenderer extends DateTimeFieldRenderer
{
	function NativeDateTimeFieldRenderer(&$parent)
	{
		$this->DateTimeFieldRenderer($parent);
	}
	
	function renderScript($field)
	{
	}

	function renderField($field)
	{
		$obj = $this->parent->data;
		$value = $obj->get($field);
		
		// JDG 10/1/2010 check for empty saved string
		$date = "";
		$time = "";
		
		if($value AND $value != '0000-00-00 00:00:00')
		{
			$date = DateTimeTypeRenderer::format($obj->reformatFromSQLDate($value), "M j, Y");
			$time = DateTimeTypeRenderer::format($obj->reformatFromSQLDate($value), "h:i A");
		}
		
		$this->_startField($field);
		echo "<input id='{$this->parent->id}_{$field}_date' type='date' name='{$this->parent->id}_{$field}_date' value='$date'>";
		echo "&nbsp;<input id='{$this->parent->id}_{$field}_time' type='time' name='{$this->parent->id}_{$field}_time' value='$time'>";
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
			
			echo "<input type='datetime' name='$field:from' value='$date_from' size='12'>";
			echo " to <input type='datetime' name='$field:to' value='$date_to' size='12'>\n";
			
			$this->_endField($field);
		}
		else
		{
			$date = $this->parent->params->get($field, $mode);
			
			$this->_startField($field);
			echo "<input type='datetime' name='$field:$mode' value='$date' size='12'>";
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
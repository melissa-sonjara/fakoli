<?php
Fakoli::usingFeature("field_renderers");

class NativeDateTimeFieldRenderer extends DateTimeFieldRenderer
{
	var $pushToSet = false;
	
	function NativeDateTimeFieldRenderer(&$parent, $field)
	{
		trace("Instantiating NativeDateTimeFieldRenderer for $field", 3);
		$this->DateTimeFieldRenderer($parent);
	}
	
	function renderScript($field)
	{
		if ($this->pushToSet)
		{
?>
<script type='text/javascript'>
window.addEvent('domready', function()
{
	document.id('<?echo $this->parent->id?>_<?echo $field?>_button').addEvent('click', function()
	{
		var now = new Date();
		var date = now.toISOString().split("T")[0];
		var time = now.toTimeString().split(" ")[0];
		document.id('<?echo $this->parent->id?>_<?echo $field?>_date').value = date;
		document.id('<?echo $this->parent->id?>_<?echo $field?>_time').value = time;
		return false;
	});
});
</script>
<?
		}
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
			
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field."_date", $label . " Date"));
			$this->parent->validator->add(new RequiredTimeValidator($field."_time", $label . " Time"));
		}
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
			$date = DateTimeTypeRenderer::format($obj->reformatFromSQLDate($value), "Y-m-d");
			$time = DateTimeTypeRenderer::format($obj->reformatFromSQLDate($value), "H:i");
		}
		
		$this->_startField($field);
		echo "<input id='{$this->parent->id}_{$field}_date' type='date' name='{$field}_date' value='$date'>";
		echo "&nbsp;<input id='{$this->parent->id}_{$field}_time' type='time' name='{$field}_time' value='$time'>";
		
		if ($this->pushToSet)
		{
			echo "&nbsp;&nbsp;<a id='{$this->parent->id}_{$field}_button' class='button' href='#'><img src='/components/mobile_settings/images/stopwatch_icon.png' alt='Push to Set'/></a>";
		}
		
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
			
			echo "<input type='datetime' name='$field:from' value='$date_from'>";
			echo " to <input type='datetime' name='$field:to' value='$date_to'>\n";
			
			$this->_endField($field);
		}
		else
		{
			$date = $this->parent->params->get($field, $mode);
			
			$this->_startField($field);
			echo "<input type='datetime' name='$field:$mode' value='$date'>";
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
		$this->parent->data->set($field, trim($_REQUEST["{$field}_date"]." ".$_REQUEST["{$field}_time"]));
		trace("DateTime: ".$this->parent->data->get($field), 1);
	}
}
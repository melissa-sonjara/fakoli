<?php
Fakoli::usingFeature("auto_form");

class TaskScheduleHelper
{
	static $periods = array(0 => array("name" => "Early Morning", "times" => "(6am-9am)"),
							1 => array("name" => "Morning", "times" => "(9am-12pm)"),
							2 => array("name" => "Early Afternoon", "times" => "(12pm-3pm)"),
							3 => array("name" => "Afternoon", "times" => "(3pm-6pm)"),
							4 => array("name" => "Evening", "times" => "(6pm-9pm)"),
							5 => array("name" => "Late Evening", "times" => "(9pm-12am)"));
	
	static function calculatePeriod($datetime)
	{
		$d = new DateTime($datetime);
		
		$day = $d->format("N") - 1;
		
		$hour = $d->format("G");
		
		$period = 0;
		
		if ($hour >= 6 && $hour < 9) $period = 0;
		else if ($hour >= 9 && $hour < 12) $period = 1;
		else if ($hour >= 12 && $hour < 15) $period = 2;
		else if ($hour >= 15 && $hour < 18) $period = 3;
		else if ($hour >= 18 && $hour < 21) $period = 4;
		else if ($hour >= 21) $period = 5;
		
		$sp = "{$day}{$period}";
		
		trace("Schedule Period for $datetime is $sp", 3);
		
		return $sp;
	}
	
	static function drawScheduleBox($id, $value, $readOnly = false)
	{
		echo "<table id='{$id}' class='schedule'><thead><tr><th>&nbsp;</th>";
		for($i = 0; $i < 7; ++$i)
		{
		$day = jddayofweek($i, 2);
			
		echo "<th class='schedule_day'>{$day}</th>";
		}
		echo "</tr></thead>";
		echo "<tbody>";
	
		foreach(TaskScheduleHelper::$periods as $period => $desc)
			{
			echo "<tr>";
			echo "<th class='schedule_period'>{$desc["name"]}<br><span class='small'>{$desc["times"]}</span></td>";
				
			for($i = 0; $i < 7; ++$i)
			{
			$idx = "{$i}{$period}";
			$class = (strpos($value, $idx) !== FALSE) ? "period selected" : "period";
				echo "<td class='{$class}' data-period='$idx'></td>";
			}
			echo "</tr>";
			}
	
			echo "</tbody></table>";
	}
}

class TaskScheduleFieldRenderer extends FieldRenderer
{
	
	function TaskScheduleFieldRenderer($parent)
	{
		$this->FieldRenderer($parent);
		$this->annotateBefore = true;
	}
	
	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)) return;
		$id = "{$this->parent->id}_{$field}";
?>
	<script type="text/javascript" src="/components/user/js/schedule_selector.js"></script>
	<script type='text/javascript'>
	window.addEvent('load', function()
	{
		new TaskScheduleSelector('<?echo $id?>_table', '<?echo $id?>');
	});
	</script>
<?
	}
	
	function drawScheduleBox($field, $readOnly = false)
	{
		$value = $this->parent->data->get($field);
		
		echo "<input type='hidden' id='{$this->parent->id}_{$field}' name='{$field}' value='{$value}'/>";
		TaskScheduleHelper::drawScheduleBox("{$this->parent->id}_{$field}_table", $value, $readOnly);
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		$this->drawScheduleBox($field);	
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);
		
		$this->drawScheduleBox($field, true);	
		
		$this->_endField($field);
	}
}


// Register the field renderer
AutoForm::registerFieldRendererClass(TaskSchedule, TaskScheduleFieldRenderer);
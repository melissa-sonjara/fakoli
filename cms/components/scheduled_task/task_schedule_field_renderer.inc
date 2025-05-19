<?php
/**\addtogroup scheduled_task
 * @{
 */


Fakoli::usingFeature("auto_form");

class TaskScheduleHelper
{	
	static function drawScheduleBox($id, $value, $readOnly = false)
	{
		echo "<table id='{$id}' class='task_schedule'><thead><tr><th>&nbsp;</th>";
		for($i = 0; $i < 7; ++$i)
		{
		$day = jddayofweek($i, 2);
			
		echo "<th class='schedule_day'>{$day}</th>";
		}
		echo "</tr></thead>";
		echo "<tbody>";
	
		for($hour = 0; $hour < 24; ++$hour)
		{
			$desc = str_pad($hour, 2, '0', STR_PAD_LEFT).":00";
			
			echo "<tr>";
			echo "<th class='schedule_period'>$desc</td>";
				
			for($i = 0; $i < 7; ++$i)
			{
				$idx = "{$i}:{$desc}";
				$class = (strpos($value, $idx) !== FALSE) ? "period selected" : "period";
				echo "<td class='{$class}' data-period='$idx'></td>";
			}
			echo "</tr>";
		}

		echo "</tbody></table>";
	}
	
	static function calculatePeriod($datetime = null)
	{
		$d = new DateTime($datetime);
	
		$day = $d->format("N") - 1;
	
		$hour = $d->format("H");
	
		$sp = "{$day}:{$hour}:00";
		
		trace("Schedule Period for $datetime is $sp", 3);
	
		return $sp;
	}	
}

class TaskScheduleFieldRenderer extends FieldRenderer
{
	
	function TaskScheduleFieldRenderer($parent)
	{
		parent::__construct($parent);
		$this->annotateBefore = true;
	}
	
	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)) return;
		$id = "{$this->parent->id}_{$field}";
?>
	<script type="text/javascript" src="/components/scheduled_task/js/task_schedule_selector.js"></script>
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
/**@}*/?>
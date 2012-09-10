<?php

class ReportTableNavigator
{
	var $tables;
	
	function ReportTableNavigator($tables)
	{
		$this->tables = $tables;
	}
	
	function writeScript()
	{
		$script = <<<ENDSCRIPT
<script type='text/javascript'>
window.addEvent('domready', function() {
	new ReportTableNavigator('custom_report');
});
</script>
ENDSCRIPT;

		return $script;
	}
	
	function drawView()
	{
		echo "<div id='report_table_navigator'>\n";
		
		echo "<h2>Choose Your Data Fields</h2>";
		
		foreach($this->tables as $table)
		{			
			$tableID = "table_{$table->class}";
			
			$tableID = "table_{$table->class}";
			$selected = $table->selected ? " selected_table" : "";
			$checked = $table->selected ? " checked='checked'" : "";
			
			echo "<div id='$tableID' class='report_table$selected'>\n";
			echo "<input type='checkbox' style='display: none' name='$tableID' value='{$table->class}'$checked/>";
			echo "<h3>".$table->getTitle()."</h3>\n";
			echo "<div class='columns'>";
			echo "<div style='display: inline-block; width: 100%'><a class='select_all' id='{$tableID}_select_all'>Select All</a><a class='show_all' id='{$tableID}_show_all'>Show All Fields</a></div>\n";

			foreach($table->columns as $column)
			{
				$columnID = "column_".codify($column->title);

				$selected = $column->selected ? " selected_column"  : "";			
				$checked = $column->selected ? " checked='checked'" : "";
				$favorite = $column->favorite ? " favorite" : "";
				
				echo "<div class='column$selected$favorite'><input type='checkbox' class='checkbox' id='$columnID' name='$columnID' value='{$column->title}'$checked/>&nbsp;{$column->title}</div>\n";
			}
			
			echo "</div></div>\n";
		}
		
		echo "</div>";
	}
}



class ReportFilterNavigator
{
	var $tables;
	
	function ReportFilterNavigator($tables)
	{
		$this->tables = $tables;
	}
	
	function writeScript()
	{
		return "";
	}
		
	function drawView()
	{
		echo "<div id='report_filter_navigator'>\n";
		
		echo "<h2>Filter Your Results</h2>";
		
		foreach($this->tables as $table)
		{
			$display = $table->selected ? "block" : "none";
			
			echo "<div class='report_filter' id='{$table->class}' style='display: $display'><h3>".$table->getTitle()."</h3>\n";
			
			echo "<div class='filter'>\n";
			echo "<table ";
			if ($this->formCSS) echo "class='{$this->formCSS}'"; 
			echo ">\n";

			$script .= $table->form->writeScript();
			$table->form->params->fromArray($this->request);
			$table->form->renderSearchFields();

			echo "</table>\n";
			echo "</div></div>";
		}		
		
		echo "</div>";
	}
}
?>
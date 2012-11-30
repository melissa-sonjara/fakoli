<?php
require_once realpath(dirname(__FILE__))."/data_view.inc";

abstract class AbstractDataImportManager
{
	var $log;
	var $objs = array();
	var $table;
	var $warnings = array();
	var $columns = array();
	var $inputTable;
	var $class_name;
	var $preview = false;

	function AbstractDataImportManager($class_name)
	{	
		$this->class_name = $class_name;
		$this->buildColumnList();
		$this->inputTable = $this->buildInputTable();
	}
	
	/**
	 * Create an array of columns as instances of DataImportColumn in
	 * the order the columns appear in the cvs file to be inported.
	 * 
	 * e.g.,
	 * $this->columns[] = new DataImportColumn($obj, "grade_id", 1, "Grade", $this->formatList($grades, "grade_name"));
	 */	
	abstract function buildColumnList();
	
	function formatList($items, $nameField)
	{
		if(count($items) == 0)
		{
			return array();
		}
		
		$out = array();
		foreach($items as $item)
		{
			$pk = $item->getPrimaryKey();
			$out[$item->$pk] = $item->$nameField;
		}
		
		return $out;
	}
		
	function import()
	{
		$fp = fopen($_FILES["import"]["tmp_name"], 'r');
	
		if (fgetcsv($fp) === FALSE) ajaxReturn("BARF!");
	
		// Use for indexing warnings
		$class_name = $this->class_name;
		$pk = $this->getPrimaryKey();
		$key = Query::create($class_name, "")->executeValue("MAX($pk)") + 1;
		
		while (($row = fgetcsv($fp)) !== FALSE)
		{
			$obj = $this->importOneRow($row, $key);
			if($obj)
			{
				$this->objs[$obj->key] = $obj;
				$key ++;
			}
		}
	}
		
	function log($text)
	{
		$this->log .= $text . "</br></br>";
	}
	
	function writeLog()
	{
		echo $this->log;
		
		$name = $this->getClassName();
		
		if(count($this->warnings))
		{
			foreach($this->warnings as $key => $warning)
			{
				echo "$name $key<br>";
				echo $warning;
			}
		}
	}
	
	function warning($obj, $text)
	{
		$key = $obj->get("key");
		$this->warnings[$key] .= $text . "</br></br>";
	}
	
	function importOneRow($row, $key)
	{
		$class_name = $this->class_name;
		$obj = new $class_name;
		
		$this->init($obj, $key);
	
		$position = 0;
		foreach($this->columns as $column)
		{
			if(!$this->preview)
			{
				$this->log("Input for {$column->field}: " . $row[$position]);
			}
			$this->setColumnValue($obj, $column, $row[$position]);
			$position++;
		}

		return $obj;
	}

	/*
	 * Initialize any fields that may not get a value
	 * in the import.
	 */
	function init(&$obj, $key)
	{
		// use for indexing	
		$obj->set("key", $key);
	}
	
	function save()
	{
		if(count($this->standards) == 0)
		{
			return;
		}
		
		foreach($this->standards as $key => $obj)
		{
			$obj->save();
			$pk = $this->getPrimaryKey();
			$name = $this->getClassName();
			$this->log("Saved $name {$obj->standard_id} {$obj->name}");
		}
	}
		
	function getPrimaryKey()
	{
		$class_name = $this->class_name;
		$obj = new $class_name;
		return $obj->getPrimaryKey();
	}
	
	function getClassName()
	{
		$class_name = $this->class_name;
		$obj = new $class_name;
		return $obj->prettifyClassName();	
	}
	
	function setColumnValue(&$obj, $column, $value)
	{
		if(!$value) return;
		$field = $column->field;
			
		if(!$column->options)
		{
			$obj->$field = $value;
			return;
		}
		
		$options = $column->options;
		if(!count($column->options)) return;
		$label = $column->label;
		
		$options = array_flip($column->options);	
		
		if(array_key_exists($value, $options))
		{
			$outValue = $options[$value];
		}
		else
		{
			$this->warning($obj, "{$label} $value not found");
			return;
		}
	
		$obj->$field = $outValue;
	}

	function buildInputTable()
	{
		$table = new DataListView($this->columns, "ImportColumns");
		$table->column("Column Name", "{field}")
		->column("Title/Label", "{label}")
		->column("Options", array($this, formatColumnValueOptions))
		->column("Position", "{position}")
		;
		
		$table->sortable = false;
		$table->paginate = false;
		$table->filter = false;
		
		return $table;
	}
	
	function writeScript()
	{
		if($this->inputTable)
		{
			$this->inputTable->writeScript();
		}
	}
	
	function drawForm()
	{
		$this->inputTable->drawView();
		
		echo "</br></br><form method='POST' action='' enctype='multipart/form-data'>
		<label>CSV file to import: </label><input type='file' name='import'/><br/><br/>
		<input type='submit' class='button' value='Import File'/>
		<input type='submit' name='preview' class='button' value='Preview Import'/>
		</form>";	
	}
	
	function preview()
	{
		$this->preview = true;
		$this->import();
		
		$this->table = $this->buildPreviewTable();
	}
	
	function buildPreviewTable()
	{
		$table = new DataListView($this->objs, "ImportedData");
		
		$position = 0;
		foreach($this->columns as $column)
		{
			$table->column($column->label, array($this, "formatColumnValue_{$position}"));
			$position++;			
		}
			
		$table->column("Warnings", array($this, formatWarnings));
		
		$table->emptyMessage = "No data was imported.";
		$table->sortable = false;
		$table->paginate = false;
		$table->filter = false;
		
		return $table;
	}
	
	function formatColumnValue($obj, $position)
	{
		$column = $this->columns[$position];
		$field = $column->field;
		
		if($column->options)
		{
			if(!$obj->$field) return "";
			return $column->options[$obj->$field] . " ({$obj->$field})";
		}
		else
		{
			return $obj->format("{{$field}}");
		}
	}
	
	function formatColumnValue_0($obj)
	{
		return $this->formatColumnValue($obj, 0);
	}

	function formatColumnValue_1($obj)
	{
		return $this->formatColumnValue($obj, 1);
	}

	function formatColumnValue_2($obj)
	{
		return $this->formatColumnValue($obj, 2);
	}
	
	function formatColumnValue_3($obj)
	{
		return $this->formatColumnValue($obj, 3);
	}
	
	function formatColumnValue_4($obj)
	{
		return $this->formatColumnValue($obj, 4);
	}
	
	function formatColumnValue_5($obj)
	{
		return $this->formatColumnValue($obj, 5);
	}
	
	function formatColumnValue_6($obj)
	{
		return $this->formatColumnValue($obj, 6);
	}
	
	function formatColumnValue_7($obj)
	{
		return $this->formatColumnValue($obj, 7);
	}
	
	function formatColumnValue_8($obj)
	{
		return $this->formatColumnValue($obj, 8);
	}
	
	function formatColumnValue_9($obj)
	{
		return $this->formatColumnValue($obj, 9);
	}

	function formatColumnValue_10($obj)
	{
		return $this->formatColumnValue($obj, 10);
	}
	
	
	function formatWarnings($obj)
	{
		$key = $obj->get("key");
		if(array_key_exists($key, $this->warnings))
		{
			$warning = $this->warnings[$key];
			return "<span style='font-size: 8px'>$warning</span>";
		}
	}
	
	function formatColumnValueOptions($objColumn)
	{
		$options = $objColumn->options;
		
		if(count($options) == 0)
		{
			return "N/A";
		}
		else
		{
			foreach($options as $key => $value)
			{
				$out[] = $value;
			}
			$out = implode("<br>", $out);
		}

		return "<span style='font-size: 10px'>$out</span>";
	}
	
}

class DataImportColumn extends DataItem
{
		var $fields = array(
		"field"		=>	String,
		"label"		=>	String,
		"position"	=>	Number,
	);
	
	var $options = array();
	
	function DataImportColumn($obj, $field, $position, $label = "", $options = array())
	{
		$this->primary_key = "field";
		$this->table = "import_column";
			
		$this->field = $field;
		$this->position = $position;
		
		$this->label = ($label) ? $label : $obj->prettifyFieldName($field);
		$this->options = $options;
	}

}
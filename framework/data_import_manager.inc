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

/**
 * Import data from a cvs file.
 * 
 * Instantiating classes must specify the details of
 * the columns in the spreadsheet in function buildColumnList
 * and any additional default values to be set inbto the
 * object in function init.
 * 
 * The page using the manager should contain the following code:

$mgr = new MyImportManager();

if ($method == "POST")
{
	if($_POST["import"])
	{
		$mgr->import();
	}
	else
	{
		$mgr->preview();		
	}
}
else
{
	$script .= $mgr->writeScript();
	$mgr->drawForm();
}

 */
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

	/**
	 * 
	 * @param String $class_name - name of the DataItem
	 * class being imported
	 */
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
	 * if the input column will provide a name value and the import
	 * manager will retrieve its key value, then instantiate the
	 * column as follows:
	 * 
	 * $this->columns[] = new DataImportColumn($obj, "grade_id", 1, "Grade", $this->formatList($grades, "grade_name"));
	 * 
	 * If the input column provides the value to be stored (e.g., "name") then
	 * instantiate the column as follows:
	 * 
	 * $this->columns[] = new DataImportColumn($obj, "name", 2));
	 * 
	 */	
	abstract function buildColumnList();
	
	/**
	 * Use this function to build an array of option key/value
	 * pairs for lookup.
	 * 
	 * @param Array $items - array of DataItem objects in the lookup table
	 * @param String $nameField - the field in the table that supplies the name
	 * that is meant to match the input value of the spreadsheet.
	 */
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
		
	/**
	 * Import and save the data.
	 */
	function import()
	{
		$fp = fopen($_FILES["import"]["tmp_name"], 'r');
	
		if (fgetcsv($fp) !== FALSE)
		{
			$this->loadData($fp);
	
			$this->save();
		
			$this->writeLog();
		}
		else
		{
			echo "<div class='error'>Cannot open data file.</div>\n";
		}
	}
	
	/**
	 * Load the data from the spreadsheet (cvs file) into the 
	 * DataItem objects.
	 */
	function loadData($fp)
	{
		// Use for indexing warnings
		$class_name = $this->class_name;
		$pk = $this->getPrimaryKey();
		$row_number = 1;
		
		while (($row = fgetcsv($fp)) !== FALSE)
		{
			$obj = $this->importOneRow($row, $row_number);
			if($obj)
			{
				$this->objs[$obj->row_number] = $obj;
				$row_number++;
			}
		}	
	}
		
	/**
	 * Store info, errors, and warnings in a log file and ouput.
	 * 
	 * @param $text
	 */
	function log($text)
	{
		$this->log .= $text . "</br></br>";
	}
	
	/**
	 * Output the log.
	 */
	function writeLog()
	{
		echo $this->log;
		
		$name = $this->getClassName();
		
		if(count($this->warnings))
		{
			foreach($this->warnings as $row_number => $warning)
			{
				echo "$name $row_number<br>";
				echo $warning;
			}
		}
	}
	
	/**
	 * Store warnings such as lookup string not found in this
	 * array that groups warnings by the object key so we can
	 * output all warnings for a specific record as a group.
	 * 
	 * @param $obj
	 * @param $text
	 */
	function warning($obj, $text)
	{
		$row_number = $obj->get("row_number");
		$this->warnings[$row_number] .= $text . "</br></br>";
	}
	
	/**
	 * Given a data row, set the value into each field of the
	 * DataItem object.
	 * 
	 * @param $row
	 * @param $row_number
	 */
	function importOneRow($row, $row_number)
	{
		$class_name = $this->class_name;
		$obj = new $class_name;
		
		$this->init($obj, $row_number);
	
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
	function init(&$obj, $row_number)
	{
		// use for indexing			
		$obj->set("row_number", $row_number);
	}
	
	function save()
	{
		if(count($this->objs) == 0)
		{
			return;
		}
		
		foreach($this->objs as $row_number => $obj)
		{
			$obj->save();
			$pk = $this->getPrimaryKey();
			$name = $this->getClassName();
			$this->log("Saved $name {$obj->$pk}");
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
	
	/**
	 * Given the input value read in, set the value
	 * into the field of the object.
	 * 
	 * If this data column is set using the options
	 * lookup and the value is not found, save the
	 * warning to display in the DataListView row.
	 * 
	 * @param $obj
	 * @param $column
	 * @param $value
	 */
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

	/**
	 * Describe the expected columns to the user above
	 * the Browse File form.
	 */
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
			$script .= $this->inputTable->writeScript();
		}
		
		return $script;
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
		
		$fp = fopen($_FILES["import"]["tmp_name"], 'r');
	
		if (fgetcsv($fp) !== FALSE)
		{
			$this->loadData($fp);

			$table = $this->buildPreviewTable();
		
			echo $this->log;
	
			$table->drawView();
		}
		else
		{
			echo "<div class='error'>Cannot open data file.</div>\n";
		}	
	}
	
	/**
	 * Display in a DataListView the values read in and set into the 
	 * data objects and show a Warnings column to list values
	 * not found in the lookup table.
	 */
	function buildPreviewTable()
	{
		$table = new DataListView($this->objs, "ImportedData");
		$table->column("Row", array($this, formatRowIndex));
		
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
	
	function formatRowIndex($obj)
	{
		return $obj->row_number;
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
	
	function formatColumnValue_11($obj)
	{
		return $this->formatColumnValue($obj, 11);
	}

	function formatColumnValue_12($obj)
	{
		return $this->formatColumnValue($obj, 12);
	}
	
	function formatWarnings($obj)
	{
		$row_number = $obj->get("row_number");
		if(array_key_exists($row_number, $this->warnings))
		{
			$warning = $this->warnings[$row_number];
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

		return "<div class='scrollbox' style='height: 200px; overflow: scroll; border: none'>$out</div>";
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
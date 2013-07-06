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
 * the columns in the spreadsheet and any additional default 
 * values to be set into the object in function init.
 * 
 * Create an array of columns as instances of DataImportColumn in
 * 
 * e.g.,
 * if the input column will provide a name value and the import
 * manager will retrieve its key value, then instantiate the
 * column as follows:
 * 
 * $this->column("grade_id", 1, "Grade", $this->formatList($grades, "grade_name"));
 * 
 * If the input column provides the value to be stored (e.g., "name") then
 * instantiate the column as follows:
 * 
 * $this->column("name", 2));
 * 
 * The page using the manager should contain the following code:

$mgr = new DataImportManager(class_name);
$mgr->column("name", 2);
etc. for each column

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

class DataImportManager
{
	var $class_name;			///< name of data item class to be updated
	var $id;
	var $log;
	var $objs = array();		///< Contains data read in from the input file
	var $warnings = array();	
	var $preview = false;		///< whether we are in preview mode
	var $matches = array();		///< Field(s) to use to check for a match in db - can be empty
	var $savedObjs = array();	///< Matches found in the db for data in the import file, indexed by primary key
	var $onPreProcessRow = null;	///< callback for after each row of data is loaded and before it is saved or displayed in preview table
	var $filepath = null;		///< optionally set known file path for csv file; otherwise provide form for user to specify
	var $validateDataFile;		///< optional callback to validate data contents of the file
	var $buttons = array();		///< The custom buttons collection
	var $msg = "";
	/**
	 * 
	 * @param String $class_name - name of the DataItem
	 * class being imported
	 */
	function __construct($class_name, $id = "")
	{	
		$this->class_name = $class_name;
		$this->id = ($id) ? $id : codify(strtolower($this->class_name)) . "_data_import";
	}
	
	/**
	 * Adds a data import column definition
	 * 
	 * @param string $field - column name for table imported to
	 * @param number $position - column order/position in import
	 * @param string $label - label for the column
	 * @param array $options - set of valid options for the imported data, if lookup is used
	 * @param callback or string $template - the formatting template or formatter function for the column
	 */
	function column($field, $position, $label = "", $options = array(), $template = "", $update_empty = true)
	{
		if(!$label)
		{
			$obj = new $this->class_name;
			$label =  $obj->prettifyFieldName($field);
		}
		$this->columns[] = new DataImportColumn($field, $position, $label, $options, $template, $update_empty);
		
		return $this;
	}	
	
	/**
	 * If values are set using logic that does not refer to data in a particular column,
	 * use additional with a callback function as the template.
	 * 
	 * @param string $field
	 * @param string $label
	 * @param string $template
	 * @param unknown $update_empty
	 * @return DataImportManager
	 */
	function additional($field, $label = "", $template = "", $update_empty)
	{
		if(!$label)
		{
			$obj = new $this->class_name;
			$label =  $obj->prettifyFieldName($field);
		}
		$this->columns[] = new DataImportColumn($field, 0, $label, null, $template, $update_empty);
		
		return $this;
	}
	
	function match()
	{
		foreach(func_get_args() as $field)
		{
			$this->matches[$field] = true;
		}
	}
	
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
		$fp = $this->openFile();		
		if(!$fp) return;
		
		if($this->loadData($fp))
		{
			$this->save();
			$this->drawButtons();
			$this->writeLog();	
		}
	}
	
	/**
	 * Adds a custom button to the form.
	 *
	 * @param string $text the button label text
	 * @param string $url the URL to handle the button press
	 * @param string $confirm optional confirmation message
	 * @param boolean $isScript true if the url is javascript code to execute,
	 * 		false if it is a URL to redirect to
	 */
	function button($text, $url, $confirm = null, $isScript = false)
	{
		$this->buttons[] = array('text' => $text, 'url' => $url, 'confirm' => $confirm, 'isScript' => $isScript);
	}	
	
	/**
	 * Load the data from the spreadsheet (cvs file) into the 
	 * DataItem objects.
	 */
	function loadData($fp)
	{
		// Use for indexing warnings
		$row_number = 1;
		$first = true;
		
		while (($row = fgetcsv($fp)) !== FALSE)
		{
			if($first && $this->validateDataFile && !call_user_func($this->validateDataFile, $row))
			{
				return false;
			}
			$first = false;
			$obj = $this->importOneRow($row, $row_number);
			if($obj)
			{
				$this->objs[$obj->row_number] = $obj;
				$row_number++;
			}
		}	
		
		return true;
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
	
		foreach($this->columns as $column)
		{
			$position = $column->position;
			if($position == 0) continue;
			$this->setColumnValue($obj, $column, $row[$position-1]);
		}
		
		// Load additional columns after in case their value depends on rows
		// loaded from spreadsheet
		foreach($this->columns as $column)
		{
			if($column->position > 0) continue;
			$this->setAdditionalValue($obj, $column);	
		}
				
		$obj = $this->preProcessRow($obj);
		
		return $obj;
	}
	
	/**
	 * If implementing classes may wish to check if the imported
	 * row should be kept or discarded. For example, check if
	 * the record already exists in the db. To discard, return 
	 * null.
	 * 
	 * @param $obj
	 */
	function preProcessRow($obj)
	{
		if(count($this->matches))
		{
			$savedObj = $this->findMatch($obj);
			if($savedObj)
			{			
				$pk = $this->getPrimaryKey();
				$this->savedObjs[$savedObj->$pk] = $savedObj;
				$obj->$pk = $savedObj->$pk;
				$this->setFilter($obj, $savedObj);
				if(!$obj->getFilter()) return null;	
			}
		}
		
		if ($this->onPreProcessRow)
		{
			call_user_func($this->onPreProcessRow, $obj);
		}
	
		return $obj;
	}
	
	function getSavedObj($obj)
	{
		$pk = $this->getPrimaryKey();
	
		if(array_key_exists($obj->$pk, $this->savedObjs))
		{
			return $this->savedObjs[$obj->$pk];
		}
		
		return null;
	}
	
	/**
	 * Check the db for a record that matches the values for the
	 * field(s) given in the matches array.
	 * 
	 * @param obj $obj
	 */
	function findMatch($obj)
	{
		if(!count($this->matches))
		{
			return null;
		}

		foreach($this->matches as $field => $dump)
		{
			$constraint[] = $field . "=:$field";
		}
		
		$query = Query::create($this->class_name, "WHERE " . implode(" AND ", $constraint));
		
		foreach($this->matches as $field => $dump)
		{
			$query->bind(":$field", $obj->$field);
		}
		
		$foundObjs = $query->execute();

		return (count($foundObjs)) ? $foundObjs[0] : null;
	}
	
	/**
	 * If a match to a saved obj is found, set the filter to include
	 * only those fields that have changed.
	 * 
	 * @param obj $obj
	 * @param obj $old
	 * @return obj
	 */
	function setFilter($obj, $old)
	{
		$fields = $obj->getFields();
		$changed = false;
	
		$obj->filter = new InclusionFilter();
		$old->filter = new InclusionFilter();
		foreach($fields as $field => $type)
		{
			$column = $this->getColumn($field);
			if(!$column) continue;
			$value = $obj->$field;
			if(($value || $column->update_empty) && $obj->$field != $old->$field)
			{
				$changed = true;
				$obj->filter->add($field);
				$old->filter->add($field);
			}
		}
	
		if(!$changed)
		{
			$obj->filter = null;
			$old->filter = null;
		}
	}
	
	function getColumn($field)
	{
		if(!count($this->columns)) return false;
		
		if(count($this->columns))
		{
			foreach($this->columns as $column)
			{
				if($column->field == $field)
				{
					return $column;
				}
			}
		}
			
		return null;
	}
	
	/*
	 * Initialize any fields that may not get a value
	 * in the import.
	 * 
	 * e.g. set user_id, date_created, etc.
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
			$pk = $this->getPrimaryKey();
			$new = false;
			
			if(!$obj->$pk)
			{
				$new = true;
			}
			$obj->save();
			$this->log($this->formatSaveLogText($row_number, $obj, $new));
		}
	}
	
	/**
	 * For each row saved, output text stating the row was updated and
	 * which fields set. If new row, write inserted.
	 * 
	 * @param number $row_number
	 * @param obj $obj
	 * @param boolean $new
	 * @returns string
	 */
	function formatSaveLogText($row_number, $obj, $new)
	{
		$pk = $this->getPrimaryKey();
		$name = $this->getClassName();
		$fields = $obj->getFields();
		$out = array();
		
		if(!$new)
		{
			$filter = $obj->getFilter();
			foreach($fields as $field => $type)
			{
				if($filter && !$filter->isExcluded($field))
				{
					$out[] = $field . " " . $obj->$field;
				}
			}
			$text = "<b>Updated</b> $name from row number $row_number $pk {$obj->$pk} " . implode(" ", $out);
		}
		else
		{
			foreach($fields as $field => $type)
			{
				$out[] = $field . " " . $obj->$field;
			}
				
			$text = "<b>Inserted</b> into $name from row number $row_number $pk {$obj->$pk} " . implode(" ", $out);
		}
		
		return $text;
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
			
		if(!$column->options && !is_callable($column->template))
		{
			$obj->$field = $value;
			return;
		}
		else if(!$column->options && is_callable($column->template))
		{
			$obj->$field = $value;
			$obj->$field = $this->format($column, $obj);
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
	
	function setAdditionalValue($obj, $column)
	{
		$field = $column->field;
		$obj->$field = $this->format($column, $obj);
	}

	
	/**
	 * Describe the expected columns to the user above
	 * the Browse File form.
	 */
	function buildInputTable()
	{
		$table = new DataListView($this->columns, $this->id);
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
		return "";
	}
	
	/**
	 * Display the expected input column layout and show a File upload
	 * renderer for the user to select the csv file to be imported.
	 */
	function drawForm()
	{
		$table = $this->buildInputTable();
		$table->drawView();
		
		echo "</br></br><form method='POST' action='' enctype='multipart/form-data'>
		<label>CSV file to import: </label><input type='file' name='csv_file'/><br/><br/>
		<input type='submit' name='import' class='button' value='Import File'/>
		<input type='submit' name='preview' class='button' value='Preview Import'/>
		</form>";	
	}
	
	/**
	 * Load the data into preview objects and display in a table.
	 */
	function preview()
	{
		$this->preview = true;
		
		$fp = $this->openFile();		
		if(!$fp) return;
		
		if($this->loadData($fp))
		{
			$table = $this->buildPreviewTable();
			$this->drawButtons();
			echo $this->log;
			$table->drawView();
		}
	}
	

	/**
	 * Draws any additional buttons specified in the calling script.
	 *
	 */
	function drawButtons()
	{
		$buttons = array();
		
		foreach($this->buttons as $button)
		{
			$url = ($button['isScript']) ? $button['url'] : "go('{$button['url']}');";
			
			if ($button['confirm'])
			{
				$link = "if (confirm('".jsSafe($button['confirm'])."')) $url; return false;";
			}
			else
			{
				$link = "$url; return false;";
			}
			
			if(preg_match("/import=1/", $url) && !count($this->objs))
			{
				continue;
			}
			$buttons[] = "<input type='button' class='{$this->buttonCSS}' onclick=\"$link\" value=\"{$button['text']}\"/>";
		}

		echo "<div class='button_row'>" . implode("&nbsp;&nbsp;", $buttons) . "</div>";
	}
		
	function openFile()
	{
		$filepath = $this->getFilePath();
		$fp = fopen($filepath, 'r');
		
		if (fgetcsv($fp) == FALSE)
		{
			echo "<div class='error'>Cannot open data file {$filepath}.</div>\n";
			return null;
		}
		
		return $fp;
	}
	
	/**
	 * Implementing classes may wish to retrieve differently
	 * (e.g., document library)
	 * 
	 * @returns String
	 */
	function getFilePath()
	{
		return (!$this->filepath) ? $_FILES["csv_file"]["tmp_name"] : $this->filepath;
	}
	
	/**
	 * Display in a DataListView the values read in and set into the 
	 * data objects and show a Warnings column to list values
	 * not found in the lookup table.
	 */
	function buildPreviewTable()
	{
		$pObjs = $this->createPreviewData();
			
		$pk = $this->getPrimaryKey();
		$table = new DataListView($pObjs, $this->id);
		$table->column("Row", array($this, formatRowIndex));
		$table->column($pk, "{{$pk}}");
		
		foreach($this->columns as $column)
		{
			$field = $column->field;
			$table->column($column->label, "{{$field}}");
		}
			
		$table->column("Warnings", array($this, formatWarnings));
		
		$table->onStartRow = array($this, previewRowStyle);		
		$table->emptyMessage = "No data to be imported.";
		$table->sortable = false;
		$table->paginate = false;
		$table->filter = false;
		
		return $table;
	}
	
	function previewRowStyle($obj)
	{
		$pk = $this->getPrimaryKey();
		if(!$obj->$pk)
		{
			return "data_import_row_new";
		}
	}
	
	/**
	 * Clone each of the objs that would be saved to the db on import
	 * and set into a preview object that has the formatted value for
	 * each field and any warning info or other details for display.
	 * 
	 * @return array of objects
	 */
	function createPreviewData()
	{
		$filter = new InclusionFilter();
		foreach($this->columns as $column)
		{
			$filter->add($column->field);
		}

		$pObjs = array();
		if(count($this->objs))
		{
			foreach($this->objs as $obj)
			{
				$pObj = clone $obj;
				$pObj->filter = $filter;
				foreach($this->columns as $column)
				{
					$pObj->filter->add($column->field);
					$pObj->set($column->field, $this->formatPreviewValue($pObj, $column));
				}
				
				$pObjs[] = $pObj;
			}
		}
		
		return $pObjs;
	}
	
	function formatPreviewValue($obj, $column)
	{
		$field = $column->field;
		$value = "";
		$oldValue = $this->getOldValue($obj, $field);
		
		if($oldValue)
		{
			$obj->fields[$field] = HTML;
			$value = "<b>";
		}
		
		if($column->options)
		{
			if(!$obj->$field) return "";
			$value .= $column->options[$obj->$field] . " (" . $this->format($column, $obj) . ")";
		}
		else
		{
			$value .= $this->format($column, $obj);
		}
		
		if($oldValue)
		{
			$value .= "</b></br><i>{$oldValue}</i>";
		}
		
		return $value;
	}
	
	/**
	 * If we found a saved obj in the db that matches the row to be imported,
	 * and the field's value has changed, return the old value.
	 * 
	 * @param obj $obj
	 * @param string $field
	 * @return string
	 */
	function getOldValue($obj, $field)
	{
		$old = $this->getSavedObj($obj);
		if($old)
		{
			$filter = $old->getFilter();
			if($filter && !$filter->isExcluded($field))
			{
				$oldValue .= $old->$field;
			}
		}
			
		return $oldValue;
	}
	
	function format($column, $obj)
	{
		if (is_callable($column->template))
		{
			return call_user_func($column->template, $obj);
		}
		else return $obj->format($column->template);
	}
	
	function formatRowIndex($obj)
	{
		return $obj->row_number;
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
		"update_empty"	=>	Boolean,
	);
	
	var $options = array();
	var $template;
		
	/**
	 * Creates a data import column definition
	 * 
	 * @param string $field - column name for table imported to
	 * @param number $position - column order/position in import
	 * @param string $label - label for the column
	 * @param array $options - optional set of valid options for the imported data, if lookup is used
	 * @param callback or string $template - optional the formatting template or formatter function for the column
	 * @param boolean update_empty - whether to save updated values that are empty, overriding existing nonempty values
	 */
	function DataImportColumn($field, $position, $label = "", $options, $template, $update_empty = true)
	{
		$this->primary_key = "field";
		$this->table = "import_column";

		$this->field = $field;
		$this->label = $label;
		$this->position = $position;
		$this->template = ($template) ? $template : "{{$field}}";
		$this->options = $options;
		$this->update_empty = $update_empty;
	}

}
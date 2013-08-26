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
 * Requires specifying the columns to be imported and their
 * position in the csv file.
 * 
 * You may wish to instantiate a helper file for custom
 * handling.
 * 
 * e.g.,
 *  
$helper = new UserImportHelper();

$mgr = new DataImportManager(SiteUser, "import_users");
$mgr->column("first_name", 1);
$mgr->column("last_name", 2);
$mgr->column("email", 3);
$mgr->column("role", 4);

$mgr->match("email");

$mgr->onPreProcessRow = array($helper, preProcessRow);
$mgr->onSaveComplete = array($helper, sendEmail);

$script .= $mgr->writeScript();
$mgr->drawView();

 * If a column needs special handling such as finding a key based on
 * a text string in a reference table, use a callback.
 * 
 * If the input column will provide a name value and the import
 * manager will retrieve its key value, then instantiate the
 * column as follows:
 * 
 * $this->column("grade_id", 1, "Grade", $this->formatList($grades, "grade_name"));
 * 
 */
require_once realpath(dirname(__FILE__))."/data_view.inc";

class DataImportManager
{
	var $class_name;			///< name of data item class to be updated
	var $id;
	var $log;
	var $objs = array();		///< Contains data read in from the input file
	var $import = false;
	var $preview = false;		///< whether we are in preview mode
	var $matches = array();		///< Field(s) to use to check for a match in db - can be empty
	var $savedObjs = array();	///< Matches found in the db for data in the import file, indexed by primary key
	var $onPreProcessRow = null;	///< callback for after each row of data is loaded and before it is saved or displayed in preview table
	var $filepath = null;		///< optionally set known file path for csv file; otherwise provide form for user to specify
	var $validateDataFile;		///< optional callback to validate data contents of the file
	var $buttons = array();		///< The custom buttons collection
	var $onSaveComplete = null;	///< Callback event handler that is fired after all the imported rows are saved
	var $field = "csv_file";
	var $cssClass = "";
	var $table;
	
	/**
	 * 
	 * @param String $class_name - name of the DataItem
	 * class being imported
	 */
	function __construct($class_name, $id = "")
	{	
		$this->class_name = $class_name;
		$this->id = ($id) ? $id : codify(strtolower($this->class_name)) . "_data_import";
		
		if($_POST["import"])
		{
			$this->import = true;
		}
	}
	
	/**
	 * Adds a data import column definition
	 * 
	 * @param string $field - column name for table imported to
	 * @param number $position - column order/position in import
	 * @param string $label - label for the column
	 * @param array $options - set of valid options for the imported data, if lookup is used
	 * @param callback $importer - optional custom import function for the column
	 */
	function column($field, $position, $label = "", $options = array(), $importer = "", $update_empty = true)
	{
		if(!$label)
		{
			$obj = new $this->class_name;
			$label =  $obj->prettifyFieldName($field);
		}
		$this->columns[] = new DataImportColumn($field, $position, $label, $options, $importer, $update_empty);
		
		return $this;
	}	
	
	/**
	 * If values are set using logic that does not refer to data in a particular column,
	 * use additional with a callback function as the template.
	 * 
	 * @param string $field
	 * @param string $label
	 * @param string $template
	 * @param boolean $update_empty
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
	 * When instantiating a DataImportColumn, use this function to build an array of 
	 * option key/value pairs for lookup.
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
			$this->importCell($column, $obj, $row[$position-1]);
		}
		
		// Load additional columns after in case their value depends on rows
		// loaded from spreadsheet
		foreach($this->columns as $column)
		{
			if($column->position > 0) continue;
			$this->importCell($column, $obj, 0);	
		}
				
		$obj = $this->preProcessRow($obj);
		
		return $obj;
	}
	
	/**
	 * If implementing classes may wish to add custom logic
	 * for handling a row before it is saved or displayed in
	 * preview.
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
			call_user_func($this->onPreProcessRow, $obj, $savedObj);
		}
	
		return $obj;
	}
	
	/**
	 * Called by the build preview table method.
	 * If during import, a record was found in the db that matched the
	 * given object, that match was saved to the savedObjs array.
	 * 
	 * @param obj $obj
	 * @return multitype:|NULL
	 */
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

		if ($this->onSaveComplete)
		{
			call_user_func_array($this->onSaveComplete, array($this));
		}
		
		return true;
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
		
		foreach($this->columns as $column)
		{
			$warning = $column->getWarning($obj);
			if($warning)
			{
				$text .= "<span class='warning'>{$warning}</warning>";
			}
		}
		
		return $text;
	}
		
	function getPrimaryKey()
	{
		$class_name = $this->class_name;
		$obj = new $class_name;
		return $obj->getPrimaryKey();
	}
	
	/**
	 * Get the display (prettified) version of the class name
	 * for the import.
	 */
	function getClassName()
	{
		$class_name = $this->class_name;
		$obj = new $class_name;
		return $obj->prettifyClassName();	
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
			->column("Options", "{formatOptions()}")
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
	
	function drawView()
	{
		$file = $this->getFilePath();
		
		$class = ($this->cssClass) ? " class='{$this->cssClass}'" : "";
		echo "<div id='data_import'{$class}>\n";
		
		if(!$file)
		{
			$this->drawForm();
		}
		else if(!$this->import)
		{
			$this->preview();
		}
		else 
		{
			$this->import();
		}
		echo "</div>\n";
	}
	
	/**
	 * Load the data into preview objects and display in a table.
	 */
	function preview()
	{
		$this->preview = true;
		$this->uploadDataImportFile();

		$fp = $this->openFile();
		if(!$fp) return;
		
		if($this->loadData($fp))
		{
			$this->table = $this->buildPreviewTable();
		}
		
		if(count($this->table->items))
		{
			echo "<form method='POST' action='' enctype='multipart/form-data'>\n";
			echo "<input type='hidden' name='{$this->field}' value='{$this->filepath}'/>\n";
			$this->drawButtons();
			echo "</form>\n";
		}
		
		echo $this->log;
		$this->table->drawView();
		
		if(!count($this->table->items) && !$this->filepath)
		{
			$this->drawForm();
		}

		$this->drawButtons();
	}
	
	/**
	 * Display the expected input column layout and show a File upload
	 * renderer for the user to select the csv file to be imported.
	 */
	function drawForm()
	{
		$table = $this->buildInputTable();
		echo "<p><label>Description of columns expected in import file:</label></p>\n";
		$table->drawView();
		
		echo "</br></br><form method='POST' action='' enctype='multipart/form-data'>
		<label>CSV file to import: </label><input type='file' name='{$this->field}'/><br/><br/>
		<input type='submit' name='import' class='button' value='Import File'/>
		<input type='submit' name='preview' class='button' value='Preview Import'/>
		</form>";	
	}
	
	function uploadDataImportFile()
	{
		global $config;
		
		$file = $_FILES[$this->field]["tmp_name"];
		
		if(!$file)
		{
			return;
		}
		
		$fullpath = $config["uploadbase"] . DIRECTORY_SEPARATOR . basename($file);
	
		if (file_exists($fullpath))
		{
			unlink($fullpath);
		}
	
		trace("Uploading $file to $fullpath", 3);
	
		move_uploaded_file($file, $fullpath);
		chmod($fullpath, 0755);
	
		$this->filepath = $fullpath;
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
		
		if($this->preview && $this->table && count($this->table->items))
		{
			array_unshift($buttons, "<input type='submit' name='import' class='button' value='Import File'/>\n");
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
		if($this->filepath)
		{
			return $this->filepath;
		}
		else if($_FILES[$this->field]["tmp_name"])
		{
			return $_FILES[$this->field]["tmp_name"];
		}
		else if($_POST[$this->field])
		{
			return $_POST[$this->field];
		}
		
		return null;
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
		$table->column(prettify($pk), "{{$pk}}");
		
		foreach($this->columns as $column)
		{
			$field = $column->field;
			$table->column($column->label, "{{$field}}");
		}
		
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
					$pObj->set($column->field, $this->formatPreviewCell($pObj, $column));
				}
				
				$pObjs[] = $pObj;
			}
		}
		
		return $pObjs;
	}
	
	/**
	 * When building the preview table, format the column using the
	 * supplied DataImportColumn preview template callback.
	 * 
	 * @param obj $obj
	 * @param obj $column
	 * @return mixed
	 */
	function formatPreviewCell($obj, $column)
	{
		$old = $this->getSavedObj($obj);
				
		if(is_callable($column->preview_template))
		{
			return call_user_func($column->preview_template, $obj, $old);
		}
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
	function importCell($column, $obj, $value)
	{
		if (is_callable($column->importer) && $column->position > 0)
		{
			return call_user_func($column->importer, $obj, $value);
		}
		else if (is_callable($column->importer) && $column->position == 0)
		{
			return call_user_func($column->importer, $obj);
		}
		
		else $column->import($obj, $value);
	}
	
	function formatRowIndex($obj)
	{
		return $obj->row_number;
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
	
	var $options = array();		///< optional - from related table or DataItem array whose value must match imported value
	var $importer;				///< optional callback to handle customized login on import of a column
	var $template;				///< formatting template for preview display of the data item field
	var $preview_template;		///< formatting template for preview display of the entire cell
	var $warnings = array();	///< store warnings indexed by the object row number pseudo field
		
	/**
	 * Creates a data import column definition
	 * 
	 * @param string $field - column name for table imported to
	 * @param number $position - column order/position in import
	 * @param string $label - label for the column
	 * @param array $options - optional set of valid options for the imported data, if lookup is used
	 * @param callback $importer - optional for handling customizezd data import logic for the column
	 * @param boolean update_empty - whether to save updated values that are empty, overriding existing nonempty values
	 */
	function __construct($field, $position, $label = "", $options, $importer, $update_empty = true)
	{
		$this->primary_key = "field";
		$this->table = "import_column";

		$this->field = $field;
		$this->label = $label;
		$this->position = $position;
		$this->importer = $importer;
		$this->options = $options;
		$this->update_empty = $update_empty;
		
		if($this->options && !$importer)
		{
			$this->importer = array($this, importOption);
		}
		
		if($this->options)
		{
			$this->preview_template = array($this, formatOptionPreview);	
		}
		else
		{
			$this->preview_template = array($this, formatPreview);
		}
		
		$this->template = "{{$field}}";
	}

	/**
	 * For fields whose value is derived from a provided options array, 
	 * format the display of the column in the preview table.
	 * 
	 * @param obj $obj
	 * @param obj $old
	 * @return string
	 */
	function formatOptionPreview($obj, $old)
	{
		$field = $this->field;
	
		$value = $this->startFormatPreview($obj, $old, $field);
		
		if(!$value)
		{
			$value .= $this->options[$obj->$field] . " (" . $obj->format($this->template) . ")";
		}
		
		$value .= $this->endFormatPreview($old, $field);
		
		return $value;	
	}
	
	/**
	 * If the value has changed, show the new value in bold.
	 * If we have a warning for this object and this column,
	 * display it in the preview.
	 * 
	 * @param obj $obj
	 * @param obj $old
	 * @param string $field
	 * @return string
	 */
	function startFormatPreview($obj, $old, $field)
	{
		$warning = $this->getWarning($obj);
		if($warning)
		{
			$obj->fields[$field] = HTML;
			$value = "<span class='warning'>{$warning}</warning>\n";	
		}
	
		if($this->isChanged($old, $field))
		{
			$obj->fields[$field] = HTML;
			$value .= "<b>";
		}
		
		return $value;
	}
	
	function getWarning($obj)
	{
		if(array_key_exists($obj->row_number, $this->warnings))
		{
			return 	$this->warnings[$obj->row_number];	
		}
		
		return "";
	}
	
	/**
	 * If the value has changed, show the old value in the table
	 * preview.
	 * 
	 * @param obj $old
	 * @param string $field
	 * @return string
	 */
	function endFormatPreview($old, $field)
	{
		if($this->isChanged($old, $field))
		{
			$value .= "</b></br><i>" . $old->format($this->template) . "</i>";
		}	
	
		return $value;
	}
	
	/**
	 * Determine if the value has changed. We can discern this using
	 * the filter b/c for objects for which a match in the db was found,
	 * we include in the inclusion filter only those fields whose value
	 * has changed.
	 * 
	 * @param obj $old
	 * @param string $field
	 * @return boolean
	 */
	function isChanged($old, $field)
	{
		if(!$old) return false;

		$filter = $old->getFilter();
		if($filter && !$filter->isExcluded($field))
		{
			return true;
		}
		
		return false;
	}
	
	/**
	 * Default preview table formatter.
	 * 
	 * @param obj $obj
	 * @param obj $old
	 * @return string
	 */
	function formatPreview($obj, $old)
	{
		$field = $this->field;

		$value = $this->startFormatPreview($obj, $old, $field);
	
		$value .= $obj->format($this->template);
		
		$value .= $this->endFormatPreview($old, $field);
	
		return $value;
	}
	
	/**
	 * If no customized callback function for setting the columns value
	 * on import is provided for a column with options, use this import
	 * logic.
	 * 
	 * @param obj $obj
	 * @param string $value
	 */
	function importOption($obj, $value)
	{
		$options = $this->options;
		if(!count($options)) return;
		$label = $this->label;
		
		$options = array_flip($this->options);
		
		if(array_key_exists($value, $options))
		{
			$obj->$field = $options[$value];
		}
		else
		{
			$this->warning[$obj->row_number] = "{$label} $value not found";
		}
	}
	
	/**
	 * Called by input table - show the set of options that represent
	 * valid input for the column.
	 */
	function formatOptions()
	{
		if(!count($this->options))
		{
			return "N/A";
		}

		foreach($this->options as $key => $value)
		{
			$out[] = $value;
		}
		$out = implode("<br>", $out);
		
		return "<div class='scrollbox' style='height: 200px; overflow: scroll; border: none'>$out</div>";
	}
	
	/**
	 * Default importer for columns
	 * 
	 * @param obj $obj
	 * @param string $value
	 */
	function import($obj, $value)
	{
		$field = $this->field;
		$obj->$field = $value;
	}
}
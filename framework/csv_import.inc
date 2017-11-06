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

require_once realpath(dirname(__FILE__)."/data_item.inc");
require_once realpath(dirname(__FILE__)."/transaction.inc");

/**
 * CSVImport provides a generic method for importing Comma-Separated Value
 * files into DataItem mapped database tables. These files can be easily
 * exported from Excel, Access and other office and database applications.
 * 
 * In order to use a CSV file for an import it must contain a first row
 * with the column names being the associated field names in your target
 * DataItem.
 * 
 * To use this class, instantiate it with the name of the DataItem class 
 * you will be importing. You can set various options to manage the import.
 * 
 * index_field specifies which field is used to determine the identity of matching
 * items already in the database
 * 
 * truncate specifies whether you want to truncate the database table before
 * importing the data.
 * 
 * update specifies whether data in the import file should update matching 
 * items already in the database. If set to false then matching import records
 * are skipped
 * 
 * onPreImportItem is a callback function that is called for each item after the data values
 * have been read from the file but before the item has been persisted to the database.
 * You can use this callback to manipulate fields during the import process.
 * 
 * onPostImportItem is a callback function that is called for each item after
 * persisting in the database. You can use this callback to perform additional
 * manipulation, such as building cross-reference records.
 * 
 * ignoreUnknown this flag specifies whether an error should be thrown if 
 * columns are found in the CSV file that do not match defined database fields
 * in your DataItem object.
 * 
 * @author andy
 *
 */
class CSVImport
{
	var $class;
	var $index_field = null;
	var $truncate = false;
	var $update = false;
	var $ignoreUnknown = false;
	var $disablePrimaryKey = false;
	var $onPreImport = null;
	var $onPostImport = null;
	var $lines = 0;
	var $imported = 0;
	var $limit = -1;
	var $tx;
	
	function __construct($class)
	{
		$this->class = $class;
	}
	
	function import($file = null)
	{
		if (!$file)
		{
			$file = $_FILES["importFile"]["tmp_name"];
		}
		
		$tx = new DataTransaction();
		
		try
		{
			$proto = new $this->class;
			$proto->joinTransaction($tx);
			
			if ($this->truncate)
			{
				$proto->deleteAll();
			}
			
			if ($this->disablePrimaryKey)
			{
				$proto->disablePrimaryKey();
			}
			
			$indexed = array();
			
			if ($this->index_field)
			{
				$indexed = IndexedQuery::create($this->class, "", $this->index_field);
			}
			
			$fp = fopen($file, "r");
			
			$fields = fgetcsv($fp);
			
			$fileIdx = -1;
			
			$i = 0;
			
			trace("Importing fields: ".implode(",", $fields), 3);
			
			// First column must contain field names that match the target class
			
			foreach($fields as $field)
			{
				if (!$proto->hasField($field) && !$this->ignoreUnknown)
				{
					throw new FakoliException("Unrecognized column: $field");
				}
				
				if ($this->index_field && $field == $this->index_field)
				{
					$fileIdx = $i;
					break;
				}
				
				$i++;
			}
	
			if ($fileIdx < 0 && !$this->ignoreUnknown && !$this->onPreImportItem)
			{
				throw new FakoliException("'{$this->index_field}' column not present");
			}
			
			$this->lines = 0;
			$this->imported = 0;
			
			while($values = fgetcsv($fp))
			{
				$this->lines++;
				
				if ($this->limit > -1 && $this->imported >= $this->limit) continue;
				
				$item = new $this->class;
				$item->joinTransaction($tx);
				
				$i = 0;
				foreach($fields as $field)
				{
					$item->set($field, $values[$i++]);
				}
	
				if ($this->onPreImport)
				{
					trace("** Calling onPreImport", 3);
					
					call_user_func($this->onPreImport, $item);
				}
				
				if ($this->index_field && array_key_exists($values[$fileIdx], $indexed))
				{
					if ($this->update)
					{
						$src = $indexed[$values[$fileIdx]];
						foreach($item->getFields() as $f)
						{
							if (!isset($item->$f)) $item->set($f, $src->get($f));
						} 
					}
					else
					{
						trace("Skipping record {$values[$fileIdx]}", 3);
						continue;
					}
				}
				
				if ($fileIdx)
				{
					trace("Importing record {$values[$fileIdx]} at line {$this->lines}", 3);
				}
				else
				{
					trace("Importing record at line $iterations", 3);
				}
				
				$item->save();
				
				$this->imported++;
				
				if ($this->onPostImport)
				{
					call_user_func($this->onPostImport, $item);
				}
			}
			
			
			if ($this->disablePrimaryKey)
			{
				$proto->enablePrimaryKey();
			}
				
			trace ("No more values from CSV", 3);		
		
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
	}

	function drawForm()
	{
?>
<form method="POST" action=""  enctype='multipart/form-data'>
<label for="importFile">Import File (CSV Format): </label>
<input type="file" name="import_file"></input>
<br/>
<input type="checkbox" class="checkbox" name="truncate" value="1"/>&nbsp;Delete existing records<br/>
<br/>
<input type="submit" value="Import Records" class='button'></input>
</form>
<?
	}		
}

?>
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

require_once realpath(dirname(__FILE__)."/search_form.inc");
require_once realpath(dirname(__FILE__)."/data_view.inc");

class APIHelper
{
	var $class;
	var $constraints;
	var $searchFilter;
	var $outputFilter;
	var $format = "json";
	
	function __construct($class, $searchFilter = null, $outputFilter = null, $constraints = "")
	{
		$this->class = $class;
		$this->searchFilter = $searchFilter;
		$this->outputFilter = $outputFilter;
		$this->constraints = $constraints;
		$this->format = isset($_GET["_format"]) ? $_GET["_format"] : "json";
	}
	
	function query()
	{
		$target = new $this->class;
		$target->filter = $this->searchFilter;
		
		$parameters = new SearchParameters($target);
		$parameters->fromGET();
		
		$constraints = (Sconstraints) ? $constraints : "WHERE 1=1 ";
		
		$query = $parameters->generateConstraint();
		
		$results = Query::create($this->class, $query)
						->filter($this->outputFilter)
						->execute();
		
		switch($this->format)
		{
		case "json":
			
			header("Content-Type: text/json");
			echo toJSON($results);
			break;
			
		case "xml":
			
			header("Content-Type: text/xml");
			echo toXML($this->class."List", $results);
			break;
			
		case "excel":
			
			$this->outputExcel($results);
			break;
			
		case "csv":
			
			$this->outputCSV($results);
			break;
			
		default:
			
			throw new FakoliException("Unknown format");			
		}
	}
	
	private function outputExcel($results)
	{
		$view = new DataListView($results);
		if (count($results))
		{
			$obj = $results[0];
			
			foreach($obj->fields as $field => $type)
			{
				if ($obj->filter && $obj->filter->isExcluded($field)) continue;
				$view->column($field, "{".$field."}");
			}
		}
		
		$view->writeExcelFile($this->class ."Results.xls");
	}			

	private function outputCSV($results)
	{
		$file = fopen('php://temp/maxmemory:'. (12*1024*1024), 'r+');
		if (count($results))
		{
			$obj = $results[0];
			$columns = array();
			foreach($obj->fields as $field => $type)
			{
				if ($obj->filter && $obj->filter->isExcluded($field)) continue;
				$columns[] = $field;
			}

			fputcsv($file, $columns);
			
			foreach($results as $obj)
			{
				$values = array();
				foreach($obj->fields as $field => $type)
				{
					if ($obj->filter && $obj->filter->isExcluded($field)) continue;
					$values[] = $obj->get($field);
				}

				fputcsv($file, $values);
			}
			
			rewind($file);
			$output = stream_get_contents($file);
			fclose($file);
			
			header("Content-Type: text/csv");
			echo $output;
		}
	}
}

?>
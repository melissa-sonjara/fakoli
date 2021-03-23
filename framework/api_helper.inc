<?php
/**\defgroup api API Support Utilities
 * @{
 */

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

/**
 * Base class for APIFormatters. Extend this class and implement
 * the format() method to provide your custom data formatter.
 * @author andy
 *
 */
abstract class APIFormatter
{
	var $parent;

	function getClass()
	{
		return $this->parent->class;
	}
	
	/**
	 * Override this function to provide your format definition.
	 * The results should be sent to the output buffer, via 'echo'
	 * or equivalent.
	 * 
	 * @param array $items the list of DataItems to be formatted
	 */
	public abstract function format($items);
}

/**
 * Standard JSON formatter - formats items as a JSON serialized array of objects.
 */
class APIJSONFormatter extends APIFormatter
{
	public function format($items)
	{
		header("Content-Type: text/json");
		echo toJSON($items);
	}
}	

/**
 * Standard XML formatter - formats items as XML in Fakoli's canonical DataItem form.
 * @author andy
 */
class APICanonicalXMLFormatter extends APIFormatter
{
	public function format($items)
	{
		header("Content-Type: text/xml");
		echo toXML($this->getClass()."List", $items);
	}
}

/**
 * Excel Formatter - formats items as a Microsoft Excel file.
 * @author andy
 */
class APIExcelFormatter extends APIFormatter
{
	public function format($items)
	{
		$view = new DataListView($items);
		if (count($items))
		{
			$obj = $items[0];
				
			foreach($obj->getFields() as $field => $type)
			{
				if ($obj->filter && $obj->filter->isExcluded($field)) continue;
				$view->column($field, "{".$field."}");
			}
		}
		
		$view->writeExcelFile($this->getClass()."Results.xls");
	}
}

/**
 * Provides CSV output formatting for APIHelper
 * @author andy
 *
 */
class APICSVFormatter extends APIFormatter
{
	public function format($items)
	{
		$file = fopen('php://temp/maxmemory:'. (12*1024*1024), 'r+');
		if (count($items))
		{
			$obj = $items[0];
			$columns = array();
			foreach($obj->getFields() as $field => $type)
			{
				if ($obj->filter && $obj->filter->isExcluded($field)) continue;
				$columns[] = $field;
			}

			fputcsv($file, $columns);
			
			foreach($items as $obj)
			{
				$values = array();
				foreach($obj->getFields() as $field => $type)
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

/**
 * APITemplate provides a simple template mechanism for transforming
 * APIHelper output to meet 3rd-party standards.  The format is simple.
 * {classname} starts the substitution block
 * {/classname} ends the substitution block (where classname is the name
 * of the DataItemClass.
 * Within the substitution block all DataItem::format() behaviors are
 * available, including method and helper function calls.
 * 
 * The MIME type of the output defaults to XML as this is the most common
 * interchange format. However, you can easily override this when creating
 * your template instance.
 * 
 * @author andy
 *
 */
class APITemplate extends APIFormatter
{
	var $file;
	var $mimeType;
	
	/**
	 * Creates a new APITemplate
	 * @param string $file the full path to the file containing the template* 
	 * @param string $mimeType the MIME type to be returned in the HTTP headers
	 * (default MIME type is text/xml) 
	 */
	public function __construct($file, $mimeType = "text/xml")
	{
		$this->file = $file;
		$this->mimeType = $mimeType;	
	}	
	
	function format($items)
	{
		$class = $this->getClass();
		$template = file_get_contents($this->file);
		
		$expr = "\{".$class."\}(.*?)\{\/".$class."\}";
		
		$matches = array();

		if (!preg_match("/$expr/si", $template, $matches))
		{
			throw new FakoliException("Badly formatted template $file - $expr<br><pre>$template<pre>");
		}
			
		$format = $matches[1];
		
		$result = formatItems($items, $format);
		
		$out = str_replace($matches[0], $result, $template);
		
		header("Content-Type: $this->mimeType");
		echo $out;
	}
}

/**
 * APIHelper is a class designed to simplify the sharing of data between applications.
 * It allows you to very quickly implement a RESTful web service that queries a given
 * DataItem class and return the results in many different formats. Output formatting
 * is extensible, but by default supports JSON, XML (in the canonical Fakoli format),
 * Excel and CSV for all objects.
 * 
 * You can provide search and output filters to be applied to your queries, as well as
 * standard constraints as need. Finally, you can easily provide templated output
 * rendering using the easy-to-use template formatter, and full custom output rendering
 * by implementing your own APIFormatter.
 *  
 * @author andy
 *
 */
class APIHelper
{
	var $class;
	var $constraints;
	var $searchFilter;
	var $outputFilter;
	var $format = "json";
	var $formatterMap = array();
	
	/**
	 * Creates an APIHelper for the specified class.
	 * @param string $class the class name for the DataItem class
	 * @param Filter $searchFilter (optional) InclusionFilter or ExclusionFilter limiting the search fields
	 * @param Filter $outputFilter (optional) InclusionFilter or ExclusionFilter limiting the output fields
	 * @param string $constraints (optional) standard SQL constraint to apply to all queries
	 */
	function __construct($class, $searchFilter = null, $outputFilter = null, $constraints = "")
	{
		$this->class = $class;
		$this->searchFilter = $searchFilter;
		$this->outputFilter = $outputFilter;
		$this->constraints = $constraints;
		$this->format = isset($_GET["_format"]) ? $_GET["_format"] : "json";
		
		$this->registerFormat("json", new APIJSONFormatter());
		$this->registerFormat("xml", new APICanonicalXMLFormatter());
		$this->registerFormat("excel", new APIExcelFormatter());
		$this->registerFormat("csv", new APICSVFormatter());
	}
	
	/**
	 * Process an API query
	 * @throws FakoliException
	 */
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
		
		if (!array_key_exists($this->format, $this->formatterMap))
		{
			throw new FakoliException("Unknown format");
		}

		return $this->formatterMap[$this->format]->format($results);
	}
	
	/**
	 * Register a format and its associated formatter
	 * @param string $name the name of the format.
	 * @param APIFormatter $formatter the formatter object that will handle 
	 * requests in this format.
	 */
	function registerFormat($name, $formatter)
	{
		$formatter->parent = $this;
		$this->formatterMap[$name] = $formatter;
	}
}
/**@}*/?>
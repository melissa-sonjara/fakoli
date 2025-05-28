<?php
/**\addtogroup datadisplay
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

/**
 * Represents a column in a DataListView output table.
 * @author Andy Green
 */
class DataColumn
{
	var $template;				///< The formatting template or formatter function for this data column
	var $sortable;				///< Boolean value indicating whether this column is sortable
	var $title;					///< The title text for this column
	var $style;					///< Any CSS styles to be applied to this column
	var $typeHint;				///< Hint to the Excel formatter about the data type to export
	var $onExport; 				///< Optional user callback function for exporting the column.
	var $sortFormat;			///< Optional explicit format to use as the basis for sorting
	var $exportOnly = false;	///< Boolean value indicating that the column should only be rendered when exporting to Excel
	
	/**
	 * Creates a new DataColumn. Generally this function should not be invoked directly,
	 * but should be accessed through the DataListView::column() method.
	 * @param string $title the title for the colum
	 * @param mixed  $template the formatting template or formatter function for the column
	 * @param boolean $sortable boolean indicating whether the column is sortable
	 * @param string $style any CSS styles to be appled to the column
	 * @param string $typeHint hint for cell type to use when exporting to Excel
	 * @param mixed $onExport optional callback function to format value for Excel 
	 * @param string $sortFormat Optional explicit format to use as the basis for sorting
	 *
	 * @return DataColumn a new DataColumn instance
	 */
	function __construct($title, $template, $sortable = true, $style = null, $typeHint = null, $onExport = null, $sortFormat = null)
	{
		$this->template = $template;
		$this->sortable = $sortable;
		$this->title = $title;
		$this->style = $style;
		$this->typeHint = $typeHint;
		$this->onExport = $onExport;
		$this->sortFormat = $sortFormat;
	}
	
	/**
	 * Outputs the text for the column using the given row. This function should not be invoked
	 * directly, but is invoked as part of the DataListView's rendering operations.
	 * @param $row the object for the row being rendered
	 * @return string the contents of the rendered cell.
	 */
	function format($row)
	{
		if (is_callable($this->template))
		{
			return call_user_func($this->template, $row);
		}
		else return $row->format($this->template);
	}
}

/**
 * Abstract base class for footer column definitions.
 * @author andy
 */
abstract class FooterColumn
{
	var $colspan;	///< The colspan for this column
	var $style;		///< Any CSS styles to be applied to this column
	
	function __construct($style = "", $colspan = 1)
	{
		$this->colspan = $colspan;
		$this->style = $style;
	}
	
	/**
	 * Renderering function for the footer column.
	 * @return string
	 */
	abstract function format();	
}

/**
 * Text footer column definition.
 * @author andy
 */
class FooterTextColumn extends FooterColumn
{
	var $text;	///< The text to display in this footer column
	
	/**
	 * Creates a new FooterTextColumn
	 * @param $text the text to display
	 * @param $style any CSS classes or inline styles to apply
	 * @param $colspan the colspan for the column
	 */
	function __construct($text = "", $style = "", $colspan = 1)
	{
		$this->text = $text;
		parent::__construct($style, $colspan);
	}
	
	/**
	 * Renderering function for the footer column.
	 * @return string
	 */
	function format()
	{
		return $this->text;
	}
}
	
/**
 * Value footer column definition.
 * @author andy
 */
class FooterValueColumn extends FooterColumn
{
	var $callback;	///< Callback function or object method used to calculate the value.
	
	/**
	 * Creates a new FooterValueColumn
	 * @param $callback Callback function or object method used to calculate the value for this column.
	 * @param $style any CSS classes or inline styles to apply
	 * @param $colspan the colspan for the column
	 */
	function __construct($callback, $style = "", $colspan = 1)
	{
		$this->callback = $callback;		
		parent::__construct($style, $colspan);
	}
	
	/**
	 * Renderering function for the footer column.
	 * @return string
	 */
	function format()
	{
		return call_user_func($this->callback);
	}
}

class FooterTotalColumn extends FooterColumn
{
	var $total = 0;
	var $template = "";
	var $field = "";
	
	/**
	 * 
	 * @param string $field - the field to be totals
	 * @param string $template - the formatting template
	 * @param string $style 
	 * @param number $colspan 
	 */
	function __construct($field, $template = "", $style = "", $colspan = 1, $typeHint = Number)
	{
		$this->field = $field;
		$this->typeHint = $typeHint;
		
		/*
		 * If the calling function puts a "$" before the
		 * field name, then mark this as type currency and
		 * output the "$" before the total.
		 */
		if (!is_callable($this->field) && strpos($this->field, "$", 0) !== FALSE)
		{
			$this->typeHint = Currency;
			$this->field = substr($this->field, 1); 
		}
			
		/*
		 * In case the field has template formatting instructions,
		 * move those out to the template variable so that
		 * the field var is just the field and can be totalled
		 * correctly.
		 */
		$matches = array();
		if (preg_match("/:(.*?)}/", $this->field, $matches))
		{
			$this->template = $matches[1];
			$this->field = str_replace($matches[1], "", $this->field);
		} 
		
		parent::__construct($style, $colspan);
	}
	
	function onStartRow($item)
	{
		if (is_callable($this->field))
		{
			$val = call_user_func($this->field, $item);
		}
		else 
		{
			$val = $item->format($this->field);
		}
		
		// Make the number a raw number value before adding
		$val = str_replace(",", "", $val);
		
		$this->total += $val;
	}
	
	function format()
	{
		if(!$this->typeHint || $this->typeHint == Number)
		{
			$renderer = new NumberTypeRenderer();
		}
		elseif($this->typeHint == Currency)
		{
			$renderer = new CurrencyTypeRenderer();
			$out = "$";
		}
		
		$out .= $renderer->format($this->total, $this->template);
		return $out;
	}
}
?>

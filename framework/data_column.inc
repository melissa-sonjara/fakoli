<?php

/**
 * Represents a column in a DataListView output table.
 * @author Andy Green
 */
class DataColumn
{
	var $template;	///< The formatting template or formatter function for this data column
	var $sortable;	///< Boolean value indicating whether this column is sortable
	var $title;		///< The title text for this column
	var $style;		///< Any CSS styles to be applied to this column
	
	/**
	 * Creates a new DataColumn. Generally this function should not be invoked directly,
	 * but should be accessed through the DataListView::column() method.
	 * @param unknown_type $title the title for the colum
	 * @param unknown_type $template the formatting template or formatter function for the column
	 * @param unknown_type $sortable boolean indicating whether the column is sortable
	 * @param unknown_type $style any CSS styles to be appled to the column
	 * @return DataColumn a new DataColumn instance
	 */
	function DataColumn($title, $template, $sortable = true, $style = null)
	{
		$this->template = $template;
		$this->sortable = $sortable;
		$this->title = $title;
		$this->style = $style;
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
	
	function FooterColumn($style = "", $colspan = 1)
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
	function FooterTextColumn($text = "", $style = "", $colspan = 1)
	{
		$this->text = $text;
		$this->FooterColumn($style, $colspan);
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
	function FooterValueColumn($callback, $style = "", $colspan = 1)
	{
		$this->callback = $callback;		
		$this->FooterColumn($style, $colspan);
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

?>
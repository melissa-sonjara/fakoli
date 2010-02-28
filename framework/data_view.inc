<?php
require_once realpath(dirname(__FILE__))."/join.inc";
require_once realpath(dirname(__FILE__))."/excel.inc";

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
 * DataListView displays a list of DataItems (or InnerJoinResults) in tabular format.
 * The table is optionally sortable and filterable by column, and can be automatically
 * paginated. The full table is sent to the client and sorting, filtering and pagination
 * are handled at the client via Javascript.
 * 
 * DataListViews can also be rendered directly into binary Microsoft Excel format via the 
 * writeExcelFile() method.
 * 
 * @author andy
 *
 */
class DataListView
{
	var $columns;		///< The column definitions for the table output.
	var $cssClass;		///< Optional CSS class to apply to the table
	var $id;			///< ID of the table in the output script.
	var $sortable;		///< Specifies whether to enable client-side sorting for this table
	var $pageSize;		///< Specifies the number of items per page - set to zero for unpaginated
	var $cssStyle;		///< Optional additional inline styles to apply to the table
	var $emptyMessage;	///< Message to display when no items are present in the data set.
	var $onStartRow;	///< Callback hook that gets called at the start of each row.
	var $onDrawfooter;	///< Callback hook that gets called after all rows have been drawn.
	var $filter;		///< Specifies whether to enable client-side filtering for this table. 
	
	/**
	 * Creates a new DataListView object.
	 * 
	 * @param array $items the array of DataItems or InnerJoinResults to be displayed
	 * @param string $id the client-side ID for this table
	 * @param string $cssClass Optional CSS class to apply to the table
	 * @param string $sortable Specifies whether to enable client-side sorting for this table
	 * @param integer $pageSize Specifies the number of items per page - set to zero for unpaginated
	 * @param boolean $filter Specifies whether to enable client-side filtering for this table. 
	 */
	function DataListView($items, $id, $cssClass = "list", $sortable = true, $pageSize = 0, $filter = false)
	{
		$this->items = $items;
		$this->id = $id;
		$this->cssClass = $cssClass;
		$this->sortable = $sortable;
		$this->filter = $filter;
		$this->pageSize = $pageSize;
		$this->cssStyle = null;
		$this->emptyMessage = "No matching items.";
		$this->onStartRow = null;
		
		return $this;
	}

	/**
	 * Adds a column definition to the DataListView.
	 * @param $title the tilte of the column
	 * @param $format the formatter for the column, either as a formatting string or callback function
	 * @param $sortable whether this column is sortable
	 * @param $style any inline styles specific to this column
	 * @return DataListView returns the current instance, allowing function call chaining for the column definitions.
	 */
	function column($title, $format, $sortable = false, $style = null)
	{
		$this->columns[] = new DataColumn($title, $format, $sortable, $style);
		return $this;
	}

	/**
	 * Sets the client-side ID for this DataListView. 
	 * @param $id the ID to use. Passing in null will generate a random ID string.
	 */
	function setID($id = null)
	{
		if ($this->id) return;
		if ($id)
		{
			$this->id = $id;
			return;
		}
		
		$this->id = makeRandomString(6);
	}
	
	/**
	 * Automatically build the table columns from the given DataItem. Not recommended except for quick and dirty
	 * pages. This method is called automatically if no columns have been defined before drawing the view.
	 * @param $item the item to use as a template for the columns.
	 */
	function autoPopulate($item)
	{
		if (count($this->columns) > 0) return;

		$fields = $item->getFieldArray();
		foreach($fields as $field)
		{
			if ($item->primary_key == $field) continue;
			
			$this->column($item->prettifyFieldName($field), "{".$field."}", $true);
		}
	}
	
	/**
	 * Write the Javascript to implement the table output.
	 * @return string containing the required Javascript.
	 */
	function writeScript()
	{
		$this->setID(); // Set to a default ID if none was specified.
		
		$script = 
		"\t<script type='text/javascript' src='/js/sorting_table.js'></script>\n".
	   	"\t<script type='text/javascript' src='/js/paginating_table.js'></script>\n".  
		"\t<script type='text/javascript' src='/js/filtering_table.js'></script>\n";
		
		$constructor = "";
		if (!$this->sortable && $this->pageSize <= 0 && !$this->filter)
		{
			return "";
		}
		
		if ($this->pageSize > 0)
		{
			$constructor = "\n\t\tvar {$this->id}_paginator = new PaginatingTable('{$this->id}', '{$this->id}_paginator', {per_page: {$this->pageSize}});";
		}
		else
		{
			$constructor = "\n\t\tvar {$this->id}_paginator = false;";
		}

		if ($this->filter !== false)
		{
			$constructor .= "\n\t\tvar {$this->id}_filter = new FilteringTable('{$this->id}', '{$this->id}_filter', {column: {$this->filter}, paginator: {$this->id}_paginator});";		
		}
		else
		{
			$constructor .= "\n\t\tvar {$this->id}_filter = false;";
		}

		if ($this->sortable)
		{
			$constructor .= "\n\t\tvar {$this->id}_sort = new SortingTable('{$this->id}', { paginator: {$this->id}_paginator, filter: {$this->id}_filter});";
		}
		
		if ($constructor)
		{
			$script .=  "\t<script type='text/javascript'>\n\twindow.addEvent('domready', function()\n{";
			$script .= $constructor;
			$script .= "\n\t});\n\t</script>\n";
		}

		return $script;
	}
	
	/**
	 * Writes the HTML for the data table to the output buffer.
	 */
	function drawView()
	{
		if (count($this->items) > 0)
		{
			$this->autoPopulate($this->items[0]); // Autopopulate the column list from the first object, if not specified
			$attrs = "";
			if ($this->id)
			{
				$attrs .= " id='{$this->id}'";
			}
			
			if ($this->cssClass)
			{
				$attrs .= " class='{$this->cssClass}";
				if ($this->sortable) $attrs .=" sortable";
				$attrs .= "'";
			}
			
			if ($this->cssStyle)
			{
				$attrs .= " style='{$this->cssStyle}'";
			}
		
?>
		<div style="float: right" class="paginator"><ul id="<?echo $this->id?>_paginator"></ul></div>
		<div style="float: left" class="filter" id="<?echo $this->id?>_filter"></div>
		<table<?echo $attrs?>>
		 <thead>
		  <tr>
<?
			foreach($this->columns as $column)
			{
				$attrs = "";
				$cssClass = "";
				if ($column->style)
				{
					if (preg_match("/^[\\w_]*$/", $column->style))
					{
						$cssClass = $column->style;
					}
					else
					{
						$attrs .= " style='{$column->style}'";
					}
				}
				if (!$column->sortable)
				{
					$cssClass = trim($cssClass." nosort");
				}

				if ($cssClass)
				{
					$attrs .= " class='$cssClass'";
				}
?>
			<th<?echo $attrs?>><?echo $column->title?></th>
<?
			}
?>
		  </tr>	
		 </thead>
		 <tbody>
<?
			foreach($this->items as $item)
			{
				if ($this->onStartRow)
				{
					call_user_func($this->onStartRow, $item);
				}
?>
          <tr>
<?
				foreach($this->columns as $column)
				{
					$attrs = "";
					if ($column->style)
					{
						$attrs .= " style='{$column->style}'";
					}
?>
		    <td><?echo $column->format($item)?></td>
<?
				}
?>
		  </tr>
<?
			}
?>  		
		</table>
<?
		}
		else
		{
			echo "<p><em>{$this->emptyMessage}</em></p>";
		}
	}
	
	/**
	 * Writes the data table out as an Excel File. The file is output
	 * with an 'attachment' disposition, which will prompt a save dialog in
	 * the client browser.
	 * @param $file the name of the file to be output.
	 * @return Does not return - exits script on completion to prevent corruption of the output file.
	 */
	function writeExcelFile($file)
	{
		$x = new ExcelFile($file);
		
		$c = 0;
		foreach($this->columns as $column)
		{
			$x->writeText(0, $c, $column->title);
			++$c;
		}
		
		$r = 1;
		foreach($this->items as $item)
		{
			$c = 0;
			if ($this->onStartRow)
			{
				call_user_func($this->onStartRow, $item);
			}
			
			foreach($this->columns as $column)
			{
				$val = $column->format($item);
				if (preg_match("/^[\\d\\.]+$/", $val))
				{
					$x->writeNumber($r, $c, $val);
				}
				else
				{
					// No HTML required in Excel output
					$x->writeText($r, $c, stripHTML($val));
				}
				
				++$c;
			}
			++$r;
		}
		
		$x->send();
		die();
	}
}
?>

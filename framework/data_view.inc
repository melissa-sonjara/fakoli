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

require_once realpath(dirname(__FILE__))."/data_column.inc";
require_once realpath(dirname(__FILE__))."/join.inc";
require_once realpath(dirname(__FILE__))."/excel.inc";

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
	var $columns;		///< The column definitions for the table.
	var $footerColumns;	///< The footer column definitions for the table.
	var $cssClass;		///< Optional CSS class to apply to the table
	var $id;			///< ID of the table in the output script.
	var $sortable;		///< Specifies whether to enable client-side sorting for this table
	var $pageSize;		///< Specifies the number of items per page - set to zero for unpaginated
	var $cssStyle;		///< Optional additional inline styles to apply to the table
	var $emptyMessage;	///< Message to display when no items are present in the data set.
	var $onStartRow;	///< Callback hook that gets called at the start of each row.
	var $onDrawfooter;	///< Callback hook that gets called after all rows have been drawn.
	var $filter;		///< Specifies whether to enable client-side filtering for this table. 
	var $excelFile;		///< Filename for automatic Excel export
	
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
		$this->excelFile = null;
		
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
	 * Adds a text column to the table footer.
	 * @param $text the text to display
	 * @param $style any CSS classes or inline styles to apply
	 * @param $colspan the colspan for the column
	 * @return DataListView returns the current instance, allowing function call chaining for the column definitions.
	 */
	function footerText($text = "", $style = "", $colspan = 1)
	{
		$this->footerColumns[] = new FooterTextColumn($text, $style, $colspan);
		return $this;
	}
	
	/**
	 * Adds a value column to the table footer.
	 * @param $callback Callback function or object method used to calculate the value for this column.
	 * @param $style any CSS classes or inline styles to apply
	 * @param $colspan the colspan for the column
	 * @return DataListView returns the current instance, allowing function call chaining for the column definitions.
	 */
	function footerValue($callback, $style = "", $colspan = 1)
	{
		$this->footerColumns[] = new FooterValueColumn($callback, $style, $colspan);
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
		
		if ($this->excelFile && $_GET["__excel"] == $this->id)
		{
			$this->writeExcelFile($this->excelFile);	
		}
		
		if (count($this->items) == 0) return;
		
		$script = 
		"\t<script type='text/javascript' src='/fakoli/js/sorting_table.js'></script>\n".
	   	"\t<script type='text/javascript' src='/fakoli/js/paginating_table.js'></script>\n".  
		"\t<script type='text/javascript' src='/fakoli/js/filtering_table.js'></script>\n";
		
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

			/* 
			 * JDG 6/15/2010 - if the cssStyle has a width setting, either px or
			 * &, apply that width style to the filter and paginator to avoid 
			 * formatting problems. 
			 */
			$width = '100%';
			if (preg_match("/^\\width:.*?\\d{1,3}px|width:.*?\\d{1,3}%/", $this->cssStyle, $matches))
				$width = $matches[0]; 
		
?>			
		<div style="width: <?echo $width ?>">
		<div style="float: right;" class="paginator"><ul id="<?echo $this->id?>_paginator"></ul></div>
		<div style="float: left" class="filter" id="<?echo $this->id?>_filter"></div></div>
		<table<?echo $attrs?>>
		 <thead>
		  <tr>
<?
			$first = true;
			
			foreach($this->columns as $column)
			{
				$attrs = "";
				$cssClass = "";

				if (!$column->sortable)
				{
					$cssClass = trim($cssClass." nosort");
				}

				if ($cssClass)
				{
					$attrs .= " class='$cssClass'";
				}
?>
			<th<?echo $attrs?>>
<?
				if ($this->excelFile && $first)
				{
					$qs = appendToQueryString(getFullQueryString(), "__excel={$this->id}");
?>
		<a href='<?echo $qs?>' style="float: left"><img src="/fakoli/images/msexcel_icon_small.png" alt="Excel" style="border: none; display:inline-block;vertical-align: middle"/></a>
<?			
				}
				
				$first = false;
?>
<?echo $column->title?></th>
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
					$cssClass = "";
					
					if (preg_match("/^[\\w_]*$/", $column->style))
					{
						$cssClass = $column->style;
					}
					else
					{
						$attrs .= " style='{$column->style}'";
					}
									
					if ($cssClass)
					{
						$attrs .= " class='$cssClass'";
					}
?>
		    <td<?echo $attrs?>><?echo $column->format($item)?></td>
<?
				}
?>
		  </tr>
<?
			}
?>
		</tbody>
<?
			if ($this->onDrawFooter) call_user_func($this->onDrawFooter);
			
			if (count($this->footerColumns) > 0)
			{
?>
		<tfoot>
		  <tr>		
<?
				foreach($this->footerColumns as $column)
				{
					$attrs = "";
					$cssClass = "";
					if ($column->style)
					{
						if (preg_match("/^[\\w_]*$/", $column->style))
						{
							$attrs .= " class='{$column->style}'";
						}
						else
						{
							$attrs .= " style='{$column->style}'";
						}
					}

					if ($column->colspan != 1)
					{
						$attrs .= " colspan='{$column->colspan}'";
					}
					?>
			<td<?echo $attrs?>><?echo $column->format()?></td>								
<?
				}
?>
		  </tr>
		</tfoot>	
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
				
				/* JDG 5/11/10 currency export fix */
				if (preg_match("/^[\\d\\,\\.]+$/", $val))
				{
					$val = str_replace(",", "", $val);
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
		
		$c = 0;
		
		if ($this->footerColumns)
		{
			foreach($this->footerColumns as $column)
			{
				$val = $column->format();
				/* JDG 5/11/10 currency export fix */
				if (preg_match("/^[\\d\\,\\.]+$/", $val))
				{
					$val = str_replace(",", "", $val);
					$x->writeNumber($r, $c, $val);
				}
				else
				{
					// No HTML required in Excel output
					$x->writeText($r, $c, stripHTML($val));
				}
				$c += $column->colspan;
			}
		}
				
		$x->send();
		die();
	}
}
?>

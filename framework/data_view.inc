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
	var $totalCallbacks;///< Array of callbacks for totalling columns
	var $filter;		///< Specifies whether to enable client-side filtering for this table. 
	var $excelFile;		///< Filename for automatic Excel export
	var $paginate = true; ///< Specifies whether table should be paginated (false for view only pages)
	var $summary;		  ///< Human-readable summary of the information contained in the table (for usability/accessiblity purposes)
	var $zebra;			///< Specifies whether alternate rows in the table should be striped
	// Scrolling Table parameters
	
	var $height;
	var $scrollable = false;
	var $resizable  = true;
	var $rowId = false; // whether to include an id tag for each tr row
	
	// Selection and Commands
	
	var $selectable  = false;	///< Whether the table provides a selection mechanism
	var $menu = null;			///< Context menu for selection-based operations
	var $commandMenuIcon = "/fakoli/images/data_view_menu.png"; ///< Icon to use for the command menu dropdown
	
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
		$this->summary = null;
		$this->zebra = true;
		$this->totalCallbacks = array();
		
		return $this;
	}

	function scrollable($height = 300, $resizable = true)
	{
		$this->scrollable = true;
		$this->height = $height;
		$this->resizable = $resizable;
	}
	
	/**
	 * Adds a column definition to the DataListView.
	 * @param $title the tilte of the column
	 * @param $format the formatter for the column, either as a formatting string or callback function
	 * @param $sortable whether this column is sortable
	 * @param $style any inline styles specific to this column
	 * @param $typeHint hint for cell type to use when exporting to Excel
	 * @param $onExport optional callback function to format value for Excel
	 * @return DataListView returns the current instance, allowing function call chaining for the column definitions.
	 */
	function column($title, $format, $sortable = false, $style = null, $typeHint = null, $onExport = null)
	{
		$this->columns[] = new DataColumn($title, $format, $sortable, $style, $typeHint, $onExport);
		return $this;
	}

	/**
	 * Adds a selector column to the DataListView. If no format is specified
	 * then selection is handled via a checklist box for the list objects' primary keys.
	 * @param $format (optional) format to use for the selector
	 */ 
	function selector($format = "")
	{
		if (!$format && count($this->items))
		{
			$pk = $this->items[0]->primary_key;
			$format = "<input type='checkbox' class='checkbox' name='{$pk}[]' id='{$pk}_{{$pk}}' value='{{$pk}}'/>";
		}
				
		if($format == "radio" && count($this->items) ){
			$pk = $this->items[0]->primary_key;
			$format = "<input type='radio' class='radio' name='{$pk}[]' id='{$pk}_{{$pk}}' value='{{$pk}}'/>";
		}	
		
		$this->column("&nbsp;", $format, false, "text-align: center");
		$this->selectable = true;
		return $this; 
	}
	
	function formatRowId($item)
	{
		$pk = $item->getPrimaryKey();
		return "{$pk}_" . $item->$pk;
	}	
	
	function commandMenu()
	{
		$this->menu = new ContextMenu($this->id . "_menu", "#{$this->id}_menu_button");
		$this->menu->trigger = "click";
		$this->menu->position = "element";
		
		return $this->menu;
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
	 * Sums the total for a column
	 * @param $field the field to total in the format "{field}"
	 * @param $style any CSS classes or inline styles to apply
	 * @param $colspan the colspan for the column
	 * @return DataListView returns the current instance, allowing function call chaining for the column definitions.
	 */
	function footerTotal($field, $style = "text-align: right", $colspan = 1)
	{
		$column =  new FooterTotalColumn($field, $template = "", $style, $colspan);
		$this->footerColumns[] = $column;
		$this->totalCallbacks[] = array($column, onStartRow);
		
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
		
		if ($this->excelFile && $_REQUEST["__excel"] == $this->id)
		{
			$this->writeExcelFile($this->excelFile);	
		}
		
		if (count($this->items) == 0) return;
		
		$script = 
		"\t<script type='text/javascript' src='/fakoli/js/sorting_table.js'></script>\n".
	   	"\t<script type='text/javascript' src='/fakoli/js/paginating_table.js'></script>\n".  
		"\t<script type='text/javascript' src='/fakoli/js/filtering_table.js'></script>\n".
		"\t<script type='text/javascript' src='/fakoli/js/scrolling_table.js'></script>\n";
		
		$constructor = "";
		if (!$this->sortable && $this->pageSize <= 0 && !$this->filter)
		{
			return "";
		}
		
		if ($this->pageSize > 0 AND $this->paginate)
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
			$zebra = $this->zebra ? "true" : "false";
			$constructor .= "\n\t\tvar {$this->id}_sort = new SortingTable('{$this->id}', { paginator: {$this->id}_paginator, filter: {$this->id}_filter, zebra: $zebra});";
		}
		
		if ($this->selectable)
		{
			$constructor .= "\n\t\t$('{$this->id}').getSelectedValues = function() { var values = [];	this.getElements(\"input[type='checkbox']\").each(function(e) { if (e.checked) values.push(e.value); }); return values; };";
		}

		if ($this->scrollable) 
		{
			$constructor .= "\n\t\tnew ScrollingTable('{$this->id}');";
		}
		
		if ($constructor)
		{
			$script .=  "\t<script type='text/javascript'>\n\twindow.addEvent('domready', function()\n{";
			$script .= $constructor;
			$script .= "\n\t});\n\t</script>\n";
		}

		if ($this->menu) $script .= $this->menu->writeScript();
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
				if ($this->scrollable) $attrs.= " scroll";
				$attrs .= "'";
			}
			
			if ($this->cssStyle)
			{
				$attrs .= " style='{$this->cssStyle}'";
			}

			if ($this->summary)
			{
				$attrs .= " summary='".htmlSafe($this->summary)."'";
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
		<?php 
		if($this->pageSize)
		{
			?>
			<div style="float: right;" class="paginator"><ul id="<?echo $this->id?>_paginator"></ul></div>
			<?php
		}
		
		if($this->filter)
		{
		?>
			<div style="float: left" class="filter" id="<?echo $this->id?>_filter"></div>
		<?php 
		}
		?>
		</div>
		
		<?php
			if ($this->scrollable)
			{	
				$tbodyAttrs = " style='height: ".($this->height - 24)."px'";
				echo "<div class='scrollContainer' style='height: {$this->height}px'>";
			}			
		?>
		<table border="0" cellpadding="0" cellspacing="0"<?echo $attrs?>>
		 <thead>
		  <tr>
<?php
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
				
				if (preg_match("/^\\width:.*?\\d{1,3}px|width:.*?\\d{1,3}%/", $column->style, $matches))
				{
					$attrs .= " style='{$matches[0]}'";
				} 
?>
			<th<?echo $attrs?>>
<?php
			
				if ($first)
				{
					if ($this->excelFile)
					{	
						$qs = appendToQueryString(getFullQueryString(), "__excel={$this->id}");
					}

					if ($this->excelFile && !$this->menu)
					{
?>
		<a href='<?echo $qs?>' style="float: left"><img src="/fakoli/images/msexcel_icon_small.png" alt="Excel" style="border: none; display:inline-block;vertical-align: middle"/></a>
<?									
					}
					else if ($this->menu)
					{
						if ($this->excelFile)
						{
							$this->menu->command($this->id."_excel", "Save as Excel...", $_SERVER['REQUEST_URI']."$qs", "/fakoli/images/msexcel_icon_small.png");
						}
?>
		<img id="<?echo $this->id?>_menu_button" src="<?echo $this->commandMenuIcon?>" alt="Table Menu" style="border: none; display:inline-block;vertical-align: middle"/>
<?									
					}
				}
				$first = false;
?>
<?echo $column->title?></th>
<?
			}
?>
		  </tr>	
		 </thead>
		 <tbody<?echo $tbodyAttrs?>>
<?
			foreach($this->items as $item)
			{
				$cl = "";
				if($this->rowId)
					$rowId = $this->formatRowId($item);
					
				if ($this->onStartRow)
				{
					$retval = call_user_func($this->onStartRow, $item);
					if ($retval === false) continue; // return false to indicate row should be skipped
					if ($retval) $cl = " class='$retval'"; // return a string to indicate a css class for the row
				}
				
				foreach($this->totalCallbacks as $cb)
				{
					call_user_func($cb, $item);
				}
?>
          <tr id='<?php echo $rowId ?>' <?php echo $cl?>>
<?php
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
<?php
				}
?>
		  </tr>
<?php
			}
?>
		</tbody>
<?php
			if ($this->onDrawFooter) call_user_func($this->onDrawFooter);
			
			if (count($this->footerColumns) > 0)
			{
?>
		<tfoot>
		  <tr>		
<?php
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
<?php
				}
?>
		  </tr>
		</tfoot>	
<?php
			}
?>  		
		</table>
<?php
			if ($this->scrollable) echo "</div>";
			
			if ($this->menu) echo $this->menu->writeMenu();
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
		$x = ExcelFileWriter::create($file);
		
		$c = 0;
		foreach($this->columns as $column)
		{
			$x->writeText(0, $c, stripHTML($column->title));
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
				if($column->onExport)
					$val = call_user_func($column->onExport, $item, $column);
				else
					$val = $column->format($item);
				
				switch($column->typeHint)
				{
				case String:

					$x->writeText($r, $c, stripHTML($val));
					break;
					
				case Number:
					
					$x->writeNumber($r, $c, $val);
					break;
					
				case Currency:
					
					$val = str_replace(array("$", ","), array("", ""), $val);
					$x->writeNumber($r, $c, $val);
					break;
					
				default:
					
					/* JDG 5/11/10 currency export fix */
					if (preg_match("/^[\\d\\,\\.\\$]+$/", $val))
					{
						$val = str_replace(array("$", ","), array("", ""), $val);
						$x->writeNumber($r, $c, $val);
					}
					else
					{
						// No HTML required in Excel output
						$x->writeText($r, $c, stripHTML($val));
					}
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
		ajaxReturn();
	}
}
?>

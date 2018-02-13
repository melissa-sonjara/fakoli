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
require_once realpath(dirname(__FILE__))."/facet_manager.inc";

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
class DataListView implements FacetFilterable
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
	var $tagRowCallbacks; ///< Array of callbacks for adding extra attributes to each row
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
	
	// Javascript interaction parameters
	
	var $rowId = false;					///< whether to include an id tag for each tr row
	var $rowIdFormat = null;				///< Optionally specify a custom row ID format
	var $dragReorder = false; 			///< Enabled drag & drop reordering
	var $reorderHandler = null;			///< Handler script that implements reordering records
	var $dragColumnReorder = false;		///< Enable drag & drop column reordering and show/hide capabilities
	var $columnReorderCallback = null;	///< Javascript callback function that is triggered when columns are reordered
	var $hideExcelIcon = false;			///< Suppress the automatic Excel icon if you want an external link
	var $onRowClick = null;				///< Javascript callback function that is triggered with a row is clicked
	var $onRowHover = null;				///< Javascript callback function that is triggered with a row is hovered over
	
	var $dragText = "";					///< Optional text to display above reorderable table
	// Selection and Commands
	
	var $selectable  = false;			///< Whether the table provides a selection mechanism
	var $menu = null;					///< Context menu for selection-based operations
	var $commandMenuIcon = "/fakoli/images/data_view_menu.png"; ///< Icon to use for the command menu dropdown
	var $toolbarCallbacks = array(); 		///< Override this callback to add toolbar links into the first header cell
	static $includesWritten = false;
	
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
		$this->tagRowCallbacks = array();
		
		return $this;
	}


	function getID()
	{
		return $this->id;
	}
	
	function scrollable($height = 300, $resizable = true)
	{
		$this->scrollable = true;
		$this->height = $height;
		$this->resizable = $resizable;
	}
	
	/**
	 * Turns on drag & drop item reordering for the table. This feature
	 * disables sorting, paging and filtering for the table.
	 * 
	 * @param string $handler URL of the handler script that processes the item reordering
	 */
	function enableDragReorder($handler)
	{
		$this->sortable = false;
		$this->filter = false;
		$this->pageSize = 0;
		$this->paginate = false;
		$this->dragReorder = true;
		$this->rowId = true;
		$this->zebra = false;
		
		$this->reorderHandler = $handler;
	}
	
	/**
	 * Adds a column definition to the DataListView.
	 * @param $title the title of the column
	 * @param $format the formatter for the column, either as a formatting string or callback function.
	 * (For details on format template specifications, see comments above class DataItem function format.)
	 * @param $sortable whether this column is sortable
	 * @param $style any inline styles specific to this column
	 * @param $typeHint hint for cell type to use when exporting to Excel
	 * @param $onExport optional callback function to format value for Excel
	 * @param string $sortFormat Optional explicit format to use as the basis for sorting
	 * @return DataListView returns the current instance, allowing function call chaining for the column definitions.
	 */
	function column($title, $format, $sortable = false, $style = null, $typeHint = null, $onExport = null, $sortFormat = null)
	{
		$this->columns[] = new DataColumn($title, $format, $sortable, $style, $typeHint, $onExport, $sortFormat);
		return $this;
	}

	/**
	 * Adds a column definition to the DataListView that is only output when exporting the table to Excel.
	 * Use this method for extended detail columns when space is constrained on the page but more details might
	 * be useful in an exported spreadsheet
	 * @param $title the title of the column
	 * @param $format the formatter for the column, either as a formatting string or callback function.
	 * (For details on format template specifications, see comments above class DataItem function format.)
	 * @param $sortable whether this column is sortable
	 * @param $style any inline styles specific to this column
	 * @param $typeHint hint for cell type to use when exporting to Excel
	 * @param $onExport optional callback function to format value for Excel
	 * @param string $sortFormat Optional explicit format to use as the basis for sorting
	 * @return DataListView returns the current instance, allowing function call chaining for the column definitions.
	 */
	function exportColumn($title, $format, $sortable = false, $style = null, $typeHint = null, $onExport = null, $sortFormat = null)
	{
		$column = new DataColumn($title, $format, $sortable, $style, $typeHint, $onExport, $sortFormat);
		$column->exportOnly = true;
		$this->columns[] = $column;
		return $this;
	}
	
	/**
	 * Override the column order
	 * @param array $order An array of column titles in the order they are to be displayed
	 */
	function setColumnOrder($order)
	{
		$columnsByTitle = reindexList($this->columns, "title");
		$columns = array();
		foreach($order as $title)
		{
			if (array_key_exists($title, $columnsByTitle))
			{
				$columns[] = $columnsByTitle[$title];
			}
		}
		
		$this->columns = $columns;
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
		if ($this->rowIdFormat)
		{
			return $item->format($this->rowIdFormat);
		}
		
		$pk = $item->getPrimaryKey();
		return "{$pk}_" . $item->$pk;
	}	
	
	/**
	 * Adds a ContextMenu to the table that can contain a list of operations that
	 * can be performed on subselections of the table's contents.
	 * @return ContextMenu
	 */
	function commandMenu()
	{
		$this->menu = new ContextMenu($this->id . "_menu", "#{$this->id}_menu_button");
		$this->menu->trigger = "click";
		$this->menu->position = "element";
		
		return $this->menu;
	}
	
	/**
	 * Registers a toolbar callback. These can be used to add custom controls into the first header cell of the table, next
	 * to the Excel export icon or command menu dropdown icon.
	 * 
	 * @param callable $callback the callback function or method
	 */
	function addToolbarCallback($callback)
	{
		$this->toolbarCallbacks[] = $callback;
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
	 * @param $typeHint type hint for formatting the output
	 * @return DataListView returns the current instance, allowing function call chaining for the column definitions.
	 */
	function footerTotal($field, $style = "text-align: right", $colspan = 1, $typeHint = Number)
	{
		$column =  new FooterTotalColumn($field, $template = "", $style, $colspan, $typeHint);
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
	 * Adds a row tagging handler. This function will receive the data item
	 * for each row and can add custom attributes to the row tag.
	 */
	function addFacetTaggingHandler($handler)
	{
		$this->tagRowCallbacks[] = $handler;
	}
	
	/**
	 * Get the Excel Link
	 */
	function getExcelLink()
	{
		$qs = appendToQueryString(getFullQueryString(), "__excel={$this->id}");
		return baseURI().$qs;
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
		
		if ($_REQUEST["__column_order"])
		{
			$this->setColumnOrder(explode("|", $_REQUEST["__column_order"]));
		}
		
		if ($this->excelFile && $_REQUEST["__excel"] == $this->id)
		{
			$this->writeExcelFile($this->excelFile);	
		}
		
		if (count($this->items) == 0) return;
		
		if (!DataListView::$includesWritten)
		{
			$script = 
			"\t<script type='text/javascript' src='/fakoli/js/sorting_table.js'></script>\n".
		   	"\t<script type='text/javascript' src='/fakoli/js/paginating_table.js'></script>\n".  
			"\t<script type='text/javascript' src='/fakoli/js/filtering_table.js'></script>\n".
			"\t<script type='text/javascript' src='/fakoli/js/scrolling_table.js'></script>\n";
			
			DataListView::$includesWritten = true;
		}
				
		$constructor = "";
		if (!$this->sortable && $this->pageSize <= 0 && !$this->filter && !$this->dragReorder)
		{
			return "";
		}

		$zebra = $this->zebra ? "true" : "false";
		
		if ($this->pageSize > 0 AND $this->paginate)
		{
			$constructor = "\n\t\tvar {$this->id}_paginator = new PaginatingTable('{$this->id}', '{$this->id}_paginator', {per_page: {$this->pageSize}, zebra: $zebra});";
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
			$constructor .= "\n\t\tvar {$this->id}_sort = new SortingTable('{$this->id}', { paginator: {$this->id}_paginator, filter: {$this->id}_filter, zebra: $zebra});";
		}
		
		if ($this->selectable)
		{
			$constructor .= "\n\t\tdocument.id('{$this->id}').getSelectedValues = function() { var values = [];	this.getElements(\"input[type='checkbox']\").each(function(e) { if (e.checked) values.push(e.value); }); return values; };";
		}

		if ($this->scrollable) 
		{
			$constructor .= "\n\t\tnew ScrollingTable('{$this->id}');";
		}
		
		if ($constructor || $this->dragReorder || $this->dragColumnReorder)
		{
			$script .=  "\t<script type='text/javascript'>\n\twindow.addEvent('domready', function()\n{";
			$script .= $constructor;
			
			if ($this->dragReorder)
			{
				$script .= "\n\t\tnew DraggableTable('#{$this->id} tbody', '{$this->reorderHandler}', '{$this->items[0]->primary_key}');";
			}
			
			if ($this->dragColumnReorder)
			{
				$options = ($this->columnReorderCallback) ? ", {onColumnsReordered: $this->columnReorderCallback}" : "";
				$script .= "\n\tnew DraggableColumnTable('{$this->id}'$options);";
			}
				
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
		trace("Number of Items: ".count($this->items), 3);
		
		if (count($this->items) > 0)
		{
			if (!count($this->columns)) $this->autoPopulate($this->items[0]); // Autopopulate the column list from the first object, if not specified
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
				if ($this->dragReorder) $attrs .= " reorder";
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
			$width = 'width: 100%';
			if (preg_match("/^\\width:.*?\\d{1,5}px|width:.*?\\d{1,3}%/", $this->cssStyle, $matches))
				$width = $matches[0]; 
		
?>			
		<div style="<?echo $width ?>">
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
		
		if ($this->dragReorder)
		{
			echo $this->dragText;
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
				if ($column->exportOnly) continue;
				
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
					
				if ($column->sortFormat) $attrs .= " data-sort='true'";
?>
			<th<?echo $attrs?>>
<?php
			
				if ($first)
				{
					if ($this->excelFile)
					{	
						$qs = $this->getExcelLink();
					}

					if ($this->excelFile && !$this->menu && !$this->hideExcelIcon)
					{
?>
		<a href='<?echo $qs?>' style="float: left"><img src="/fakoli/images/msexcel_icon_small.png" alt="Excel" style="border: none; display:inline-block;vertical-align: middle"/></a>
<?									
					}
					else if ($this->menu)
					{
						if ($this->excelFile)
						{
							$this->menu->command($this->id."_excel", "Save as Excel...", $qs, true, "/fakoli/images/msexcel_icon_small.png");
						}
?>
		<img id="<?echo $this->id?>_menu_button" src="<?echo $this->commandMenuIcon?>" alt="Table Menu" style="border: none; display:inline-block;vertical-align: middle"/>
<?									
					}
					
					foreach($this->toolbarCallbacks as $callback)
					{
						echo "<div style='display: inline-block; float: left'>";
						echo call_user_func($callback, $this);
						echo "</div>";
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
				$cl = array();
				if($this->rowId)
					$rowId = $this->formatRowId($item);
					
				if ($this->onStartRow)
				{
					$retval = call_user_func($this->onStartRow, $item);
					if ($retval === false) continue; // return false to indicate row should be skipped
					if ($retval) $cl[] = $retval; // return a string to indicate a css class for the row
				}
				
				$dataAttrs = array();
				foreach($this->tagRowCallbacks as $cb)
				{
					$dataAttrs = call_user_func($cb, $item, $dataAttrs);
				}
				
				$rowAttrs = "";
				foreach($dataAttrs as $name => $value)
				{
					$rowAttrs .= " ".$name."='".$value."'";
				}
				
				foreach($this->totalCallbacks as $cb)
				{
					call_user_func($cb, $item);
				}
				
				$onclick = "";
				
				if ($this->onRowClick)
				{
					$onclick =  " onclick=\"" . $item->format($this->onRowClick) . "\" onmouseover=\"this.addClass('hover');";
					if ($this->onRowHover) $onlick .= $item->format($this->onRowHover);
					$onclick .=" return false;\" onmouseout=\"this.removeClass('hover'); return false;\"";
					$cl[] = "clickable";
				}
				
				$cl = implode(" ", $cl);
				if ($cl) $cl = "class='$cl'";
				if ($rowId) $rowId = "id='$rowId' ";
?>
          <tr <?echo $rowId?><?php echo $cl?><?echo $onclick?><?echo $rowAttrs?>>
<?php
				foreach($this->columns as $column)
				{
					if ($column->exportOnly) continue;
				
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
					
					if ($column->sortFormat) $attrs .= " data-sort='".htmlentities($item->format($column->sortFormat))."'";
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

					if ($column->colspan > 1)
					{
						if (!$this->dragColumnReorder)
						{
							$attrs .= " colspan='{$column->colspan}'";
					?>
			<td<?echo $attrs?>><?echo $column->format()?></td>								
<?php
						}
						else if (preg_match("/text-align:\\s*right/i", $column->style))
						{
							for($i = 1; $i < $column->colspan; ++$i)
							{
								echo "<td>&nbsp;</td>";
							}
?>
<td<?echo $attrs?>><?echo $column->format()?></td>
<?
						}
						else
						{
?>
<td<?echo $attrs?>><?echo $column->format()?></td>
<?
							for($i = 1; $i < $column->colspan; ++$i)
							{
								echo "<td>&nbsp;</td>";
							}
						}
					}
					else
					{
?>
<td<?echo $attrs?>><?echo $column->format()?></td>
<?
					}
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
			$x->writeHeading(0, $c, stripHTMLTags(stripHTML($column->title)));
			++$c;
		}
		
		$r = 1;
		foreach($this->items as $item)
		{
			$c = 0;
			if ($this->onStartRow)
			{
				$retval = call_user_func($this->onStartRow, $item);
				if ($retval === false) continue;
			}
						
			foreach($this->totalCallbacks as $cb)
			{
				call_user_func($cb, $item);
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

					$x->writeText($r, $c, stripHTMLTags(stripHTML($val)));
					break;
					
				case Number:
					
					$x->writeNumber($r, $c, $val);
					break;
					
				case Currency:
					
					$val = str_replace(array("$", ","), array("", ""), $val);
					$x->writeCurrency($r, $c, $val);
					break;
					
				case Percentage:
					
					$x->writePercentage($r, $c, $val);
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
						$x->writeText($r, $c, stripHTMLTags(stripHTML($val)));
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
					$x->writeFooterNumber($r, $c, $val);
				}
				else
				{
					// No HTML required in Excel output
					$x->writeFooter($r, $c, stripHTMLTags(stripHTML($val)));
				}
				$c += $column->colspan;
			}
		}
				
		$x->send();
		
		session_write_close();
		exit();
	}
}
?>

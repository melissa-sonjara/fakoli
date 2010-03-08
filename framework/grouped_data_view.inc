<?php
require_once realpath(dirname(__FILE__))."/data_column.inc";
require_once realpath(dirname(__FILE__))."/join.inc";
require_once realpath(dirname(__FILE__))."/excel.inc";

/**
 * Represents a grouped collection of data within a table.
 * @author andy
 */
class DataGroup
{
	var $parent; 	///< The parent GroupedDataListView object
	var $title;	 	///< The text to display in the title for this data group
	var $key;		///< The key value for this data group
	var $expanded;	///< Boolean indicating whether this group should initially be expanded.
	var $fixed;		///< Specifies whether this group should be fixed (i.e. exempt from expand/collapse toggling).
	
	/**
	 * Creates a new DataGroup
	 * @param $parent the parent table
	 * @param $title The text to display in the title for this data group
	 * @param $key The key value for this data group
	 * @param $expanded Boolean indicating whether this group should initially be expanded.
	 * @param $fixed Specifies whether this group should be fixed (i.e. exempt from expand/collapse toggling).
	 */
	function DataGroup($parent, $title, $key, $expanded = false, $fixed = false)
	{
		$this->parent = $parent;
		$this->title = $title;
		$this->key = $key;
		$this->expanded = $expanded;
	}

	/**
	 * Draws the data group subheading within the table.
	 */
	function draw()
	{
		$cssClass = ($this->fixed || $this->parent->mode == 'fixed') ? "fixed" : ($this->expanded) ? "expanded" : "collapsed";
?>
	<tr class="<?echo $this->parent->groupClass?> <?echo $cssClass?>">
	 <td colspan="<?echo $this->parent->columnCount?>"><?echo $this->title?></td>
	</tr>
<?
	}
}

/**
 * GroupedDataListView displays a list of DataItems (or InnerJoinResults) in tabular format
 * grouped by a specific field.
 * 
 * Grouped tables cannot be sortable or filterable by column or paginated. However, the groups
 * can optionally be expandable by clicking, in either a "tree" or "accordion" style. With
 * "tree" mode, each group can be expanded or hidden independently of the others. With 
 * "accordion" mode, only one group can be expanded at a time. With "fixed" mode, all the 
 * groups are displayed and cannot be hidden.
 * 
 * GroupedDataListViews can also be rendered directly into binary Microsoft Excel format via the 
 * writeExcelFile() method.
 * 
 * @author andy
 *
 */
class GroupedDataListView
{
	var $columns;		///< The column definitions for the table.
	var $footerColumns;	///< The footer column definitions for the table.
	var $cssClass;		///< Optional CSS class to apply to the table
	var $groupClass;	///< The CSS class to apply to the group subheading rows.
	var $id;			///< ID of the table in the output script.
	var $cssStyle;		///< Optional additional inline styles to apply to the table
	var $emptyMessage;	///< Message to display when no items are present in the data set.
	var $onStartRow;	///< Callback hook that gets called at the start of each row.
	var $onDrawFooter;	///< Callback hook that gets called after all rows have been drawn.
	var $mode;			///< The display mode for the table.
	var $groups;		///< The group definitions.
	var $items;			///< The items to be displayed.
	
	/**
	 * Creates a new GroupedDataListView object.
	 * 
	 * @param array $items the array of DataItems or InnerJoinResults to be displayed
	 * @param string $id the client-side ID for this table
	 * @param string $cssClass Optional CSS class to apply to the table
	 * @param string $sortable Specifies whether to enable client-side sorting for this table
	 * @param integer $pageSize Specifies the number of items per page - set to zero for unpaginated
	 * @param boolean $filter Specifies whether to enable client-side filtering for this table. 
	 */
	function GroupedDataListView($items, $id, $cssClass = "list", $groupClass = "subheading")
	{
		$this->items = $items;
		$this->id = $id;
		$this->cssClass = $cssClass;
		$this->groupClass = $groupClass;
		$this->cssStyle = null;
		$this->emptyMessage = "No matching items.";
		$this->onStartRow = null;
		$this->onDrawFooter = false;
		$this->mode = "fixed";
	}

	/**
	 * Adds a group to the view.
	 * @param unknown_type $title the title text for the group.
	 * @param unknown_type $key the key value for the group.
	 * @param unknown_type $expanded true for expanded by default, false for collapsed by default
	 * @return GroupDataListView returns the current instance, allowing function call chaining for the group definitions.
	 */
	function group($title, $key, $expanded = false, $fixed = false)
	{
		$this->groups[] = new DataGroup($this, $title, $key, $expanded);
		return $this;
	}
	
	/**
	 * Add groups based on the supplied list of DataItems.
	 * @param $groups list of DataItems
	 * @param $format format template for the group titles
	 * @param $field the field to use for the key (defaults to the primary key of the group items if not specified)
	 */
	function groupBy($groups, $format = null, $field = "")
	{
		if (count($groups) == 0) return;
		if (!$field) $field = $groups[0]->primary_key;
		
		foreach($groups as $group)
		{
			$this->group($group->format($format), $group->$field);
		} 
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
		
		$script = "<script type='text/javascript' src='/js/grouping_table.js'></script>\n";
		
		$constructor = "";

		if ($this->mode != 'fixed')
		{
			$constructor = "\n\t\tvar {$this->id} = new GroupingTable('{$this->id}', {mode: '{$this->mode}'});";
		}

		if ($constructor)
		{
			$script .=  "\t<script type='text/javascript'>\n\twindow.addEvent('domready', function()\n\t{";
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
			
			$this->columnCount = count($this->columns);
		
?>
		<table<?echo $attrs?>>
		 <thead>
		  <tr>
<?
			foreach($this->columns as $column)
			{
				$attrs = "";
				$cssClass = "nosort";
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
			foreach($this->groups as $group)
			{
				if (array_key_exists($group->key, $this->items))
				{
					$group->draw();

					foreach($this->items[$group->key] as $item)
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
				}
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
		foreach($this->groups as $group)
		{
			if (array_key_exists($group->key, $this->items))
			{
				$x->writeText($r, 0, $group->title);
				$r++;
				
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
			}
		}

		$c = 0;
		
		foreach($this->footerColumns as $column)
		{
			$val = $column->format();
			if (preg_match("/^[\\d\\.]+$/", $val))
			{
				$x->writeNumber($r, $c, $val);
			}
			else
			{
				// No HTML required in Excel output
				$x->writeText($r, $c, stripHTML($val));
			}
			$c += $column->colspan;
		}
		
		$x->send();
		die();
	}
}
?>

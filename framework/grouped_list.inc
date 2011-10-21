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

require_once realpath(dirname(__FILE__))."/grouped_data_view.inc";

/*
 * Similar to PagedList class but provides a one-level 
 * expandable/collapsable style list, given an array of items 
 * that are grouped by a field.
 * 
 * For example, projects grouped by type. Outputs the project
 * type as a heading followed by all projects that are categorized
 * as that type. The project type heading can be expanded/collapsed.
 * 
 * Can be used for Blogs and other formatted list displays.
 */

/**
 * Represents a grouped collection of data within a table.
 * @author Janice
 *
 *
 * Note: 10/2011 - This is a work in progress.
 */


class GroupedListGroup extends DataGroup
{
	var $listTag = "ul";
	
	function GroupListGroup($parent, $title, $key, $expanded = false, $fixed = false)
	{
		parent::DataGroup($parent, $title, $key, $expanded, $fixed);
	}

	function draw()
	{
		$attrs = "";
		if ($this->CSSclass) $attrs .= " class='{$this->CSSclass}'";
		if ($this->groupStyles) $attrs .= " style='{$this->groupStyles}'";
		
		$cssClass = ($this->fixed || $this->parent->mode == 'fixed') ? "fixed" : ($this->expanded) ? "expanded" : "collapsed";

		// Output the section heading (e.g., "ul" tag)
		echo "<{$this->listTag} id=\"{$this->parent->group_id}[{$this->key}]\" class=\"node\" $attrs>\n";
		// Output the section title
		echo "{$this->title}\n";
		echo "</{$this->listTag}>\n";
	}
}


class GroupedList
{
	var $items;
	var $id;
	var $format;
	var $mode;			///< The display mode for the table: fixed, tree, or accordian
	var $groups;		///< The group definitions.
	var $CSSclass = null;
	var $styles = null;
	var $groupStyles = ""; // Styles for the group headings
	var $groupClass = "subheading"; // Class for the ul that links the li list
	var $groupedListClass; // The css class for the entire grouped list div
	var $group_id;
	
	function GroupedList($items, $id = "grouped_list", $format, $cssClass = "article_list", $groupClass = "subheading")
	{
		$this->items = $items;
		$this->id = $id;
		$this->format = $format;
		$this->cssClass = $cssClass;
		$this->groupClass = $groupClass;
		$this->cssStyle = null;
		$this->mode = "accordian";
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
		$this->groups[] = new GroupedListGroup($this, $title, $key, $expanded);
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

		$this->group_id = $field;
		
		foreach($groups as $group)
		{
			$this->group($this->format($group, $format), $group->$field);
		} 
	}

	/**
	 * Outputs the text for the groupedBy title bar using the given group dataitem
	 * object. This function should not be invoked directly, but is invoked as 
	 * part of the GroupedDataListView's rendering operations.
	 * 
	 * @param $group the dataitem object for the title bar being rendered
	 * @return title for the titel bar.
	 */	
	function format($group, $template)
	{
		if(!is_callable($template))
			return $group->format($template);
		else
			return call_user_func($template, $group);
	}
	
	
	function writeScript()
	{
		if(count($this->items) == 0) return;
		
		$script = "<script type='text/javascript' src='/fakoli/js/grouping_list.js'></script>\n";
		
		if ($this->mode != 'fixed')
		{
			$constructor = "\n\t\tvar {$this->id} = new GroupingList('{$this->id}', {mode: '{$this->mode}'});";
		}
		
		if ($constructor)
		{
			$script .=  "\t<script type='text/javascript'>\n\twindow.addEvent('domready', function()\n\t{";
			$script .= $constructor;
			$script .= "\n\t});\n\t</script>\n";
		}

		return $script;
	}
	
	function drawList()
	{
		$attrs = "";
		if ($this->CSSclass) $attrs .= " class='{$this->CSSclass}'";
		if ($this->styles) $attrs .= " style='{$this->styles}'";

		echo "<div id='grouped_list' class='{$this->groupListClass}'>\n";
		
		if(count($this->groups) == 0) return;
		
		foreach($this->groups as $group)
		{
			if (array_key_exists($group->key, $this->items))
			{
				$group->draw();
					
				$itemsInGroup = $this->items[$group->key];

				echo "<{$group->listTag} id=\"subheading[{$group->key}]\" class=\"subheading\" $attrs>\n";
	
				foreach($itemsInGroup as $item)
				{
					echo "<li style='display: list-item;'>";
		
					if (is_callable($this->format))
					{
						echo call_user_func($this->format, $item);
					}
					else echo $item->format($this->format);
					echo "</li>\n";
				}
				
				echo "</{$group->listTag}\n";
			}
		} // end for each groups as group

		echo "</div>\n";
	}
}

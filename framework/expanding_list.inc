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

/**
 * The ExpandingList class provides a simple User Interface control
 * to allow the display of list items by heading. Clicking on a heading
 * expands the body for that list item.. 
 * @author andy
 */
class ExpandingList
{
	var $items;									///< The items to be displayed in the list
	var $id;									///< The HTML id of the list
	var $titleFormat;							///< The formatter used to display the list item titles (either a format string or a callback function or method)
	var $bodyFormat;							///< The formatter used to display the list item bodies (either a format string or a callback function or method)
	var $CSSclass = "expanding_list";			///< CSS class(es) to be applied to the list
	var $styles = null;							///< Specific CSS styles to be applied to the list
	var $emptyMessage = "No items in the list";	///< Message to display when the list is empty
	var $groupTag = "h4";						///< Grouping tag type
	var $grouped = false;						///< Flag indicating whether list should be grouped
	var $dynamicLoad = false;					///< Flag indicating whether list should load content on demand. If set to true, bodyFormat should provide the URL for each item
	var $titleIDFormat = null;					///< Format template for title ID field
	var $bodyIDFormat = null;					///< Format template for body ID field
	var $scrollOnExpand = false;				///< Flag indicating whether to scroll the expanding item to the top of the screen
	var $scrollOffset = 0;						///< Offset from top of element when scrolling (to take into account fixed navigation bars, etc)
	var $isItemExpanded = null;					///< Callback hook to determine if an item should be shown expanded by default. Callback is passed the item and current index. Return true for expanded, false for collapsed.
	var $onItemStart = null;					///< Callback hook called when starting to display each item. Return false to skip the item.
	
	/**
	 * Creates a new ExpandingList control.
	 * @param array $items the array of DataItems to be displayed in the list
	 * @param string $id the HTML id of the list
	 * @param mixed $titleFormat The formatter used to display the list item titles (either a format string or a callback function or method)
	 * @param mixes $bodyFormat The formatter used to display the list item bodies (either a format string or a callback function or method)
	 * @param boolean $dynamicLoad Specifies whether to load the contents of each list element on demand. If set to true, the $bodyFormat parameter should provide the URL for each item
	 */
	function ExpandingList($items, $id = "expanding_list", $titleFormat = null, $bodyFormat = null, $dynamicLoad = false)
	{
		$this->items = $items;
		$this->id = $id;
		$this->titleFormat = $titleFormat;
		$this->bodyFormat  = $bodyFormat;
		$this->dynamicLoad = $dynamicLoad;
	}
	
	/**
	 * Writes the supporting Javascript for an expanding list control
	 * @return string
	 */
	function writeScript()
	{
		ob_start();
?>
	<script type="text/javascript">
	function <?echo $this->id?>_toggleDD(elt)
	{
		elt = document.id(elt);
		var body = elt.getNext("dd");
		
		if (elt.hasClass("expanded"))
		{
			body.dissolve();
			elt.removeClass("expanded");
		}
		else
		{
			if (body.get('data-url') && body.get('html') == '')
			{
				var cursor = elt.getStyle('cursor');

				elt.setStyle('cursor', 'progress');
				
				body.reload(function() 
				{ 
					body.reveal();
					elt.addClass("expanded");
					elt.setStyle('cursor', cursor);
<?php 
			if ($this->scrollOnExpand)
			{
?>
					window.scrollToElement(elt, <?php echo $this->scrollOffset?>);
<?php 
			}
?>
				});
			}
			else
			{
				body.reveal();
				elt.addClass("expanded");
			<?php 
			if ($this->scrollOnExpand)
			{
			?>
				window.scrollToElement(elt, <?php echo $this->scrollOffset?>);
			<?php 
			}
			?>
			}
		}	
	}

	window.addEvent('domready', function()
	{
		document.getElements("dl.expanding_list dd.expanded[data-url!='']").each(function(panel)
		{
			panel.reload();
			panel.removeClass('expanded');
		});
	});	
	</script>
<?
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}
	
	/**
	 * Generates the HTML for the expanding list
	 */
	function drawList()
	{
		if (!count($this->items))
		{
			echo "<p>{$this->emptyMessage}</p>";
			return;
		}
		
		if ($this->grouped)
		{
			$this->drawGroupedList();
			return;
		}	
		
		$attrs = "";
		if ($this->CSSclass) $attrs .= " class='{$this->CSSclass}'";
		if ($this->styles) $attrs .= " style='{$this->styles}'";
?>
			<dl id="<?echo $this->id?>"<?echo $attrs?>>
<?
		$idx = 0;
		
		foreach($this->items as $item)
		{
			if (is_callable($this->onItemStart))
			{
				$ret =call_user_func($this->onItemStart, $item);
				if ($ret === false) continue;
			}
			
			$titleID = ($this->titleIDFormat) ? $item->format(" id='{$this->titleIDFormat}'") : "";
			$bodyID =  ($this->bodyIDFormat) ? $item->format(" id='{$this->bodyIDFormat}'") : "";
		
			$expanded = false;
			if (is_callable($this->isItemExpanded))
			{
				$expanded = call_user_func($this->isItemExpanded, $item, $idx);
			}
			
			if ($expanded)
			{
				$dtState = "class='expanded {$ret}'";
				$ddState = "style=''";
			}
			else
			{
				$dtState = "class='{$ret}'";
				$ddState = "style='display: none'";
			}
			
			echo "<dt{$titleID} {$dtState} onclick='return {$this->id}_toggleDD(this);'>";
			
			if (is_callable($this->titleFormat))
			{
				echo call_user_func($this->titleFormat, $item);
			}
			else echo $item->format($this->titleFormat);
			
			echo "</dt>\n";
			
			if (is_callable($this->bodyFormat))
			{
				$body = call_user_func($this->bodyFormat, $item);
			}
			else $body = $item->format($this->bodyFormat);
			
			if ($this->dynamicLoad)
			{
				echo "<dd{$bodyID} {$dtState} {$ddState} data-url='{$body}'></dd>";
			}
			else
			{
				echo "<dd{$bodyID} {$ddState}>{$body}</dd>";				
			}	
			
			++$idx;
		}
		echo "</dl>\n";
	}
	
	/**
	 * Draws a set of grouped expanding lists.
	 */
	function drawGroupedList()
	{
		$attrs = "";
		if ($this->CSSclass) $attrs .= " class='{$this->CSSclass}'";
		if ($this->styles) $attrs .= " style='{$this->styles}'";
		
		$groupIdx = 0;
		
		foreach($this->items as $group => $items)
		{
?>
		<<?echo $this->groupTag?> class='expanding_list_group'><?echo $group?></<?echo $this->groupTag?>>		
			<dl id="<?echo $this->id?>_<?echo codify($group)?>"<?echo $attrs?>>
<?
			$idx = 0;
			
			foreach($items as $item)
			{
				if (is_callable($this->onItemStart))
				{
					if (call_user_func($this->onItemStart, $item) === false) continue;
				}
				
				$titleID = ($this->titleIDFormat) ? $item->format(" id='{$this->titleIDFormat}'") : "";
				$bodyID =  ($this->bodyIDFormat) ? $item->format(" id='{$this->bodyIDFormat}'") : "";
		
				$expanded = false;
				if (is_callable($this->isItemExpanded))
				{
					$expanded = call_user_func($this->isItemExpanded, $item, $idx, $groupIdx);
				}
				
				if ($expanded)
				{
					$dtState = "class='expanded'";
					$ddState = "style=''";
				}
				else
				{
					$dtState = "";
					$ddState = "style='display: none'";
				}
				
				echo "<dt{$titleID} {$dtState} onclick='return {$this->id}_toggleDD(this);'>";
				
				if (is_callable($this->titleFormat))
				{
					echo call_user_func($this->titleFormat, $item);
				}
				else echo $item->format($this->titleFormat);
				
				echo "</dt>\n";
				
				if (is_callable($this->bodyFormat))
				{
					$body = call_user_func($this->bodyFormat, $item);
				}
				else $body = $item->format($this->bodyFormat);
				
				if ($this->dynamicLoad)
				{
					echo "<dd{$bodyID} {$dtState} {$ddState} data-url='{$body}'></dd>";
				}
				else
				{
					echo "<dd{$bodyID} {$ddState}>{$body}</dd>";				
				}
				
				++$idx;
			}
			echo "</dl>\n";
			++$groupIdx;
		}
	}
}
?>
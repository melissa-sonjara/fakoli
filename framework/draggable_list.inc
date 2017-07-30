<?php
/**************************************************************

Copyright (c) 2017 Sonjara, Inc

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
 * DraggableList provides a simple user interface for sorting a list of items.
 * Users can click and drag items within the list. Sorting is updated on the
 * server via a sort handler that must be able to parse pairs of primary keys
 * and sort order values.
 *
 * @author andy
 */
class DraggableList
{
	var $items;
	var $id;
	var $format;
	var $listTag = "ul";						///< The HTML list tag type
	var $CSSclass = null;						///< CSS class(es) to be applied to the list
	var $styles = null;							///< Specific CSS styles to be applied to the list
	var $emptyList = "No items in the list";	///< Message to display when the list is empty
	
	var $reorderHandler = null;			///< Handler script that implements reordering records
	var $dragText = "";					///< Optional text to display above reorderable table
	
	function __construct($items, $id = "draggable_list", $format = null, $handler = null)
	{
		$this->items = $items;
		$this->id = $id;
		$this->format = $format;
		$this->reorderHandler = $handler;
	}
	
	function enableDragReorder($handler)
	{
		$this->reorderHandler = $handler;
	}
	
	function writeScript()
	{
		if (!count($this->items)) return;
		$pk = $this->items[0]->getPrimaryKey();
		
		$script = <<<ENDSCRIPT
<script type='text/javascript'>
window.addEvent('domready', function()
{
	new DraggableList('{$this->id}', '{$this->reorderHandler}', '{$pk}');
});
</script>
ENDSCRIPT;
		return $script;
	}
	
	function drawList()
	{
		if (!count($this->items))
		{
			echo "<p>{$this->emptyList}</p>";
			return;
		}

		$attrs = "";
		if ($this->CSSclass) 
		{
			$attrs .= " class='draggable {$this->CSSclass}'";
		}
		else
		{
			$attrs .= " class='draggable'";
		}
		
		if ($this->styles) $attrs .= " style='{$this->styles}'";		
?>

		<? echo $this->dragText;?>
		<<?echo $this->listTag?> id="<?echo $this->id?>"<?echo $attrs?>>
<?
		foreach($this->items as $item)
		{
			$pkTag = " data-pk='".$item->getPrimaryKeyValue()."'";	
			
			echo "<li{$pkTag}>";
			
			if (is_callable($this->format))
			{
				echo call_user_func($this->format, $item);
			}
			else echo $item->format($this->format);
			
			echo "</li>\n";
		}
		echo "</{$this->listTag}>\n";
	}
}
?>
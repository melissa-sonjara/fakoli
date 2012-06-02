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
	
	/**
	 * Creates a new ExpandingList control.
	 * @param array $items the array of DataItems to be displayed in the list
	 * @param string $id the HTML id of the list
	 * @param mixed $titleFormat The formatter used to display the list item titles (either a format string or a callback function or method)
	 * @param mixes $bodyFormat The formatter used to display the list item bodies (either a format string or a callback function or method)
	 */
	function ExpandingList($items, $id = "expanding_list", $titleFormat = null, $bodyFormat = null)
	{
		$this->items = $items;
		$this->id = $id;
		$this->titleFormat = $titleFormat;
		$this->bodyFormat  = $bodyFormat;
	}
	
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
			body.reveal();
			elt.addClass("expanded");
		}		
	}
	
	</script>
<?
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}
	
	function drawList()
	{
		if (!count($this->items))
		{
			echo "<p>{$this->emptyMessage}</p>";
			return;
		}
		
		$attrs = "";
		if ($this->CSSclass) $attrs .= " class='{$this->CSSclass}'";
		if ($this->styles) $attrs .= " style='{$this->styles}'";
?>
			<dl id="<?echo $this->id?>"<?echo $attrs?>>
<?
		foreach($this->items as $item)
		{
			echo "<dt onclick='return {$this->id}_toggleDD(this);'>";
			
			if (is_callable($this->titleFormat))
			{
				echo call_user_func($this->titleFormat, $item);
			}
			else echo $item->format($this->titleFormat);
			
			echo "</dt>\n";
			
			echo "<dd style='display: none'>";
			if (is_callable($this->bodyFormat))
			{
				echo call_user_func($this->bodyFormat, $item);
			}
			else echo $item->format($this->bodyFormat);
			echo "</dl>\n";
		}
		echo "</dl>\n";
	}
}
?>
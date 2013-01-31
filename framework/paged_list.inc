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
 * The PagedList class provides a simple User Interface control
 * to allow pagination of HTML lists of DataItems. 
 * @author andy
 */
class PagedList implements FacetFilterable
{
	var $items;									///< The items to be displayed in the list
	var $id;									///< The HTML id of the list
	var $format;								///< The formatter used to display the list items (either a format string or a callback function or method)
	var $pageSize = 10;							///< The number of items per page
	var $listTag = "ul";						///< The HTML list tag type
	var $CSSclass = null;						///< CSS class(es) to be applied to the list
	var $styles = null;							///< Specific CSS styles to be applied to the list
	var $emptyList = "No items in the list";	///< Message to display when the list is empty
	var $paginate = true; ///< Specifies whether table should be paginated (false for view only pages)
	var $writeIdTag = false;					///< whether to include an id tag for each li item
	var $tagRowCallbacks; ///< Array of callbacks for adding extra attributes to each row
	
	/**
	 * Creates a new PagedList control.
	 * @param array $items the array of DataItems to be displayed in the list
	 * @param string $id the HTML id of the list
	 * @param mixed $format The formatter used to display the list items (either a format string or a callback function or method)
	 */
	function PagedList($items, $id = "paged_list", $format = null)
	{
		$this->items = $items;
		$this->id = $id;
		$this->format = $format;
	}
	
	function getID()
	{
		return $this->id;
	}
	
	/**
	 * Adds a row tagging handler. This function will receive the data item
	 * for each row and can add custom attributes to the row tag.
	 */
	function addFacetTaggingHandler($handler)
	{
		$this->tagRowCallbacks[] = $handler;
	}
	
	function writeScript()
	{
		if(!$this->paginate) return "";
		
		ob_start();				
?>
	<script type="text/javascript">
	window.addEvent('domready', function()
	{
		var <?echo $this->id?> = new PaginatingList('<?echo $this->id ?>', '<?echo $this->id?>_paginator', {per_page: <?echo $this->pageSize ?>});
	});
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
			echo "<p>{$this->emptyList}</p>";
			return;
		}
		
		$attrs = "";
		if ($this->CSSclass) $attrs .= " class='{$this->CSSclass}'";
		if ($this->styles) $attrs .= " style='{$this->styles}'";
		if($this->paginate)
		{
?>
			<div style="float: right;" class="paginator">
			<ul id="<?echo $this->id?>_paginator"></ul>
			</div>
		<?php 
		}
		?>
			<<?echo $this->listTag?> id="<?echo $this->id?>"<?echo $attrs?>>
<?
		foreach($this->items as $item)
		{
			if($this->writeIdTag)
			{
				$idTag = "id='".$this->formatIdTag($item)."'";	
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
			
			echo "<li {$idTag}{$rowAttrs}>";
			
			if (is_callable($this->format))
			{
				echo call_user_func($this->format, $item);
			}
			else echo $item->format($this->format);
			
			echo "</li>\n";
		}
		echo "</{$this->listTag}>\n";
	}
	
	function formatIdTag($item)
	{
		$pk = $item->getPrimaryKey();
		return "{$pk}_{$item->$pk}";
	}
}
?>
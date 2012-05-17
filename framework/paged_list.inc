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

class PagedList
{
	var $items;
	var $id;
	var $format;
	var $pageSize = 10;
	var $listTag = "ul";
	var $CSSclass = null;
	var $styles = null;
	var $emptyList = "No items in the list";
	
	function PagedList($items, $id = "paged_list", $format = null)
	{
		$this->items = $items;
		$this->id = $id;
		$this->format = $format;
	}
	
	function writeScript()
	{
		ob_start();
?>
	<script type="text/javascript">
	window.addEvent('domready', function()
	{
		var <?echo $this->id?> = new PaginatingList('<?echo $this->id?>', '<?echo $this->id?>_paginator', {per_page: <?echo $this->pageSize?>});
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
?>
			<div style="float: right;" class="paginator"><ul id="<?echo $this->id?>_paginator"></ul></div>
			<<?echo $this->listTag?> id="<?echo $this->id?>"<?echo $attrs?>>
<?
		foreach($this->items as $item)
		{
			echo "<li>";
			
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
<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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
 
class ImageListView
{
	var $id;
	var $images;
	var $linkFormat;
	var $spacing = 0;
	var $styles;
	var $titleFormat = "{title}";
	
	function ImageListView($images, $linkFormat, $id = "images", $titleFormat = "")
	{
		$this->id = $id;
		$this->images = $images;
		$this->linkFormat = $linkFormat;
		$this->titleFormat = ($titleFormat) ? $titleFormat : "{title}";
	}
	
	function writeScript()
	{
		global $script;
		$script .= <<<ENDSCRIPT
	<script type="text/javascript">
	window.addEvent('domready', function()
	{
		$$(".imageGrid a").each(function(elt) 
		{		
			elt.addEvents(
			{
				'mouseover': function()
				{
					this.morph({'background-color': '#A9CDED', 'border-color': '#666666'});
				}.bind(elt),
				'mouseout': function()
				{
					this.morph({'background-color': '#eeeeff', 'border-color': '#cccccc'});
				}.bind(elt)
			});
		});		
	});
	</script>
ENDSCRIPT;

		return $script;
	}
	
	function drawGrid()
	{
		$imgMgr = new ImageManager();
		
		$this->writeScript();
?>
		<ul class="imageGrid" id="<?echo $this->id?>"<?if ($this->styles) echo " style='{$this->styles}'";?>>
	<?
	
		foreach($this->images as $image)
		{
			$thumb = $imgMgr->thumbnailLink($image->image_id, 125);
			$link = $image->format($this->linkFormat);
			
			if (is_callable($this->titleFormat))
				$caption = call_user_func($this->titleFormat, $image);
			else 
				$caption = $image->format($this->titleFormat);
			
			?>
			<li><div class="imageLink"<?if ($this->spacing) echo " style='margin: {$this->spacing}'";?>><a href="<?echo $link?>">
			<span class="wrapImage"><img src="<?echo $thumb ?> " alt="Small version of image"/></span>
			<span class="caption"><?echo $caption ?></span></a>
			</div></li>
			<?
	    }
	?>
		</ul>
	<?
	}

}

	
	
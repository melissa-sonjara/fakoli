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
 
class ImageGalleryView
{
	var $listView;
	var $id;
	
	var $images;
	var $linkFormat;
	var $spacing = 0;
	var $styles;
	
	function ImageGalleryView($images, $linkFormat, $id = "images")
	{
		$this->id = $id;
		$this->listView = new ImageListView($images, $linkFormat, $id . "_listView");
	}

	function drawGallery()
	{
		global $script;
		
		$script .= <<<ENDSCRIPT
	<script type="text/javascript" src="/fakoli/image_gallery/image_gallery.js"></script>
	<script type="text/javascript">
	var gallery;
	window.addEvent('domready', function()
	{
		gallery = new ImageGallery('$this->id');
	});
	</script>	
ENDSCRIPT;

		$width = intval((count($this->listView->images)  / 2) + 1) * 185;
		$this->listView->styles="position: absolute; width: {$width}px";
		$this->listView->spacing = $this->spacing;
		
?>
<div id="<?echo $this->id?>__left" style="width: 32px; float: left;<?echo $this->styles?>"><img id="<?echo $this->id?>_scroll_left" src="/fakoli/image_gallery/scroll_left.png" style="margin-top: 160px"/>&nbsp;</div>
<div id="<?echo $this->id?>" style="width: 560px; height: 450px; position: relative; overflow: hidden; float: left">
<?
		$this->listView->drawGrid();
?>
</div>
<div id="<?echo $this->id?>_right" style="width: 32px; float: left"><img id="<?echo $this->id?>_scroll_right" src="/fakoli/image_gallery/scroll_right.png" style="margin-top: 160px"/>&nbsp;</div>
</div>
<?
	}
}


class ImageGalleryList
{
	var $galleries;
	var $linkFormat;
	var $id;
	var $galleryView;
	
	function ImageGalleryList($galleries, $linkFormat, $id = "galleries")
	{
		$this->galleries = $galleries;
		$this->linkFormat = $linkFormat;
		
		$this->id = $id;
		$this->galleryView = new ImageGalleryView($this->collateImages(), $this->linkFormat, $this->id);
		$this->galleryView->listView->titleFormat = "{Gallery.gallery_name}";
	}
	
	function collateImages()
	{
		$this->images = array();
		
		if (count($this->galleries) == 0) return;
		
		$ids = formatItems($this->galleries, "{gallery_id}", ", ");
		$groupedImages = GroupedQuery::create(ImageRecord, "WHERE gallery_id IN (". $ids .") AND published=1", "gallery_id")
						->execute();
						
		foreach($groupedImages as $gallery_id => $images)
		{
			$this->images[] = $images[0];
		}
		
		return $this->images;
	}		
		
	function drawGallery()
	{
		if (count($this->images) == 0)
		{
			echo "<p>No photos have been published on this site.</p>";
			return;
		}
		
		$this->galleryView->drawGallery();	
	}	

	function drawList()
	{
		if (count($this->images) == 0)
		{
			echo "<p>No photos have been published on this site.</p>";
			return;
		}
		
		$this->galleryView->listView->drawGrid();	
	}	
}
	
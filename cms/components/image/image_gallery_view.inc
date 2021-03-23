<?php
/**\addtogroup image
 * @{
 */


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
	var $grid;
	var $id;
	
	var $images;
	var $linkFormat;
	var $spacing = 0;
	var $styles;
	var $titleFormat = "{title}";
	
	function ImageGalleryView($images, $linkFormat, $id = "images", $size = 125)
	{
		$this->id = $id;
		$this->grid = new ImageGrid($id . "_gallery", $size);
		$this->images = $images;
		$this->linkFormat = $linkFormat;
	}

	function drawGallery()
	{
		foreach($this->images as $image)
		{
			$this->grid->addItem($image->image_id, $image->format($this->titleFormat), $image->caption, $image->format($this->linkFormat));
		}
		
		global $script;
		
		$script .= $this->grid->writeScript();
		$this->grid->drawView();
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
		$this->galleryView->titleFormat = "{Gallery.gallery_name}";
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
		
		$this->galleryView->drawGallery();	
	}	
}
	
/**@}*/?>
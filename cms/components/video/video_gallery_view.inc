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
 
class VideoGalleryView
{
	var $listView;
	var $id;
	
	var $videos;
	var $linkFormat;
	var $spacing = 0;
	var $styles;
	var $onClick;
	
	function VideoGalleryView($videos, $linkFormat, $id = "videos", $onClick)
	{
		$this->id = $id;
		$this->listView = new VideoListView($videos, $linkFormat, $id . "_listView", "", $onClick);
	}

	function drawGallery()
	{
		global $script;
		
		$script .= <<<ENDSCRIPT
	<script type="text/javascript" src="/components/video/js/video_gallery.js"></script>
	<script type="text/javascript">
	var gallery;
	window.addEvent('domready', function()
	{
		gallery = new VideoGallery('$this->id');
	});
	</script>	
ENDSCRIPT;

		$width = intval((count($this->listView->videos)  / 2) + 1) * 185;
		$this->listView->styles="position: absolute; width: {$width}px";
		$this->listView->spacing = $this->spacing;
		
?>
<div>
<div id="<?echo $this->id?>_left" style="width: 32px; float: left;<?echo $this->styles?>"><img id="<?echo $this->id?>_scroll_left" src="/components/video/images/scroll_left.png" style="margin-top: 160px"/>&nbsp;</div>
<div id="<?echo $this->id?>" style="width: 560px; height: 450px; position: relative; overflow: hidden; float: left">
<?
		$this->listView->drawGrid();
?>
</div>
<div id="<?echo $this->id?>_right" style="width: 32px; float: left"><img id="<?echo $this->id?>_scroll_right" src="/components/video/images/scroll_right.png" style="margin-top: 160px"/>&nbsp;</div>
</div>
<?
	}
}


class VideoGalleryList
{
	var $galleries;
	var $linkFormat;
	var $id;
	var $galleryView;
	
	function VideoGalleryList($galleries, $linkFormat, $id = "galleries")
	{
		$this->galleries = $galleries;
		$this->linkFormat = $linkFormat;
		
		$this->id = $id;
		$this->galleryView = new VideoGalleryView($this->collateVideos(), $this->linkFormat, $this->id);
		$this->galleryView->listView->titleFormat = "{Gallery.gallery_name}";
	}
	
	function collateVideos()
	{
		$this->videos = array();
		
		if (count($this->galleries) == 0) return;
		
		$ids = formatItems($this->galleries, "{gallery_id}", ", ");
		$groupedVideos = GroupedQuery::create(Video, "WHERE gallery_id IN (". $ids .") AND published=1", "gallery_id")
						->execute();
						
		foreach($groupedVideos as $gallery_id => $videos)
		{
			$this->videos[] = $videos[0];
		}
		
		return $this->videos;
	}		
		
	function drawGallery()
	{
		if (count($this->videos) == 0)
		{
			echo "<p>No videos have been published on this site.</p>";
			return;
		}
		
		$this->galleryView->drawGallery();	
	}	

	function drawList()
	{
		if (count($this->videos) == 0)
		{
			echo "<p>No videos have been published on this site.</p>";
			return;
		}
		
		$this->galleryView->listView->drawGrid();	
	}	
}
	
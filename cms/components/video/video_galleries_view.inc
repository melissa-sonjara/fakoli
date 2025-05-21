<?php
/**\addtogroup video
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
 
class VideoGalleriesView
{
	var $table;
	
	function __construct($showHidden = false)
	{
		if (!$showHidden) $constraint = "WHERE hidden=0";
		
		$galleries = Query::create('VideoGallery', "$constraint ORDER BY gallery_name")->execute();
		
		$this->table = $this->buildTable($galleries);
	}
	
	function buildTable($galleries)
	{
		$table = new DataListView($galleries, "galleries");
		$table->column("Video Gallery", array($this, formatTitleLink), true, "width: 55%")
	  		->column("Owner", "{getOwnerName()}", true, "width: 30%")
	  		->column("# Videos", "{countVideos()}", true, "width: 15%;text-align: right")
	  		->column("Published", array($this, showPublished), false, "text-align: center");
	  
		$table->sortable = true;
		$table->filter = true;
		$table->pageSize = 10;
		$table->emptyMessage = "No Video Galleries have been created.";
		
		return $table;
	}
	
	function writeScript()
	{
		return $this->table->writeScript();	
	}
	
	function drawView()
	{
		$this->table->drawView();
	}
	
	function formatTitleLink($gallery)
	{
		return $gallery->format("<a href='video_gallery_form?video_gallery_id={video_gallery_id}'>{gallery_name}</a>");	
	}

	function showPublished($gallery)
	{
		$img = $gallery->published ? "on.png" : "off.png";
		$alt = $gallery->published ? "Enabled" : "Disabled";

		return "<img src='/fakoli/images/$img' alt='$alt' onclick='toggleGalleryPublished(this, {$gallery->gallery_id})' style='display: inline-block; vertical-align: middle; border: none; cursor: pointer'/>";
	}	
}
	

class AdminVideoGalleriesView extends VideoGalleriesView
{
	function __construct($showHidden = false)
	{
		parent::__construct($showHidden);	
	}
	
	function formatTitleLink($gallery)
	{
		return $gallery->format("<a href='/admin/video_gallery_form?gallery_id={gallery_id}'>{gallery_name}</a>");	
	}
	
}
	
/**@}*/?>

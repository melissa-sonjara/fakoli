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
 
class VideoListView
{
	var $id;
	var $videos;
	var $linkFormat;
	var $spacing = 0;
	var $styles;
	var $titleFormat = "{title}";
	var $onClick;
	
	function VideoListView($videos, $linkFormat, $id = "videos", $titleFormat = "", $onClick = "")
	{
		$this->id = $id;
		$this->videos = $videos;
		$this->linkFormat = $linkFormat;
		$this->titleFormat = ($titleFormat) ? $titleFormat : "{title}";
		$this->onClick = $onClick;
	}
	
	function writeScript()
	{
		global $script;
		$script .= <<<ENDSCRIPT
	<script type="text/javascript">
	window.addEvent('domready', function()
	{
		$$(".videoGrid a").each(function(elt) 
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
		$imgMgr = new VideoManager();
		
		$this->writeScript();
?>
		<ul class="videoGrid" id="<?echo $this->id?>"<?if ($this->styles) echo " style='{$this->styles}'";?>>
	<?
	
		foreach($this->videos as $video)
		{
			$thumb = $imgMgr->thumbnailLink($video->get("video_id"), 125);
			$link = $video->format($this->linkFormat);
			$onClick = $video->format($this->onClick);
			
			if (is_callable($this->titleFormat))
				$caption = call_user_func($this->titleFormat, $video);
			else 
				$caption = $video->format($this->titleFormat);
			
			?>
			<li><div class="videoLink"<?if ($this->spacing) echo " style='margin: {$this->spacing}'";?>><a href="<?echo $link?>"<? if ($onClick) echo " onClick=\"$onClick\""; ?>>
			<span class="wrapVideo"><img src="<?echo $thumb ?> " alt="Small version of video"/></span>
			<span class="caption"><?echo $caption ?></span></a>
			</div></li>
			<?
	    }
	?>
		</ul>
	<?
	}

	static function writeResizeScript()
	{
	
		$script .= <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function() { $('videos_box').resizable(); });
</script>
ENDSCRIPT;

		return $script;
	}	

}

	

class VideoGalleryListView
{
	var $table;
	var $link;
	
	function VideoGalleryListView($gallery, $link = "video_form?video_id={video_id}")
	{
		$videos = $gallery->Videos("ORDER BY title");
		$this->link = $link;
		
		$this->table = $this->buildTable($videos);
	}

	function buildTable($videos)
	{

		$table = new DataListView($videos, "videos");
		$table->column("Title", "<a href='{$this->link}'>{title}</a>", true, "width: 50%")
	  		->column("Video File", "{video_file}", true, "width: 50%");
		$table->emptyMessage = "No videos have been added to this gallery";
		$table->pageSize = 20;
		$table->sortable = true;
		$table->filter = true;
		
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
}

class AdminVideoGalleriesView extends VideoGalleriesView
{
	function AdminVideoGalleriesView()
	{
		parent::VideoGalleriesView();	
	}
	
	function formatTitleLink($gallery)
	{
		return $gallery->format("<a href='/admin/video_gallery_form?gallery_id={gallery_id}'>{gallery_name}</a>");	
	}
	
}
	
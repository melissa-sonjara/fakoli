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
			$thumb = $imgMgr->thumbnailLink($image->get("image_id"), 125);
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

	static function writeResizeScript()
	{
	
		$script .= <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function() { document.id('image_box').resizable(); });
</script>
ENDSCRIPT;

		return $script;
	}	

}

	

class ImageGalleryListView
{
	var $table;
	var $link;
	
	function ImageGalleryListView($gallery, $link = "image_form?image_id={image_id}")
	{
		$images = $gallery->Images("ORDER BY title");
		$this->link = $link;
		
		$this->table = $this->buildTable($images);
	}

	function buildTable($images)
	{

		$table = new DataListView($images, "images");
		$table->column("Title", "<a href='{$this->link}'>{title}</a>", true, "width: 40%")
	  		->column("Image File", "{image_file}", true, "width: 40%")
	  		->column("Date Taken", "{date_taken}", true, "width: 20%");
		$table->emptyMessage = "No images have been added to this gallery";
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

class ImageGalleriesView
{
	var $table;
	
	function ImageGalleriesView()
	{
		
		$galleries = Query::create(ImageGallery, "ORDER BY gallery_name")->execute();
		
		$this->table = $this->buildTable($galleries);
	}
	
	function buildTable($galleries)
	{
		$table = new DataListView($galleries, "galleries");
		$table->column("Image Gallery", array($this, formatTitleLink), true, "width: 55%")
	  		->column("Owner", "{getOwnerName()}", true, "width: 30%")
	  		->column("# Images", "{countImages()}", true, "width: 15%;text-align: right")
	  		->column("Published", array($this, showPublished), false, "text-align: center");
	  
		$table->sortable = true;
		$table->filter = true;
		$table->pageSize = 10;
		$table->emptyMessage = "No Image Galleries have been created.";
		
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
		return $gallery->format("<a href='image_gallery_form?gallery_id={gallery_id}'>{gallery_name}</a>");	
	}

	function showPublished($gallery)
	{
		$img = $gallery->published ? "on.png" : "off.png";
		$alt = $gallery->published ? "Enabled" : "Disabled";

		return "<img src='/fakoli/images/$img' alt='$alt' onclick='toggleGalleryPublished(this, {$gallery->gallery_id})' style='display: inline-block; vertical-align: middle; border: none; cursor: pointer'/>";
	}	
}

class AdminImageGalleriesView extends ImageGalleriesView
{
	function AdminImageGalleriesView()
	{
		parent::ImageGalleriesView();	
	}
	
	function formatTitleLink($gallery)
	{
		return $gallery->format("<a href='/admin/image_gallery_form?gallery_id={gallery_id}'>{gallery_name}</a>");	
	}
	
}
	
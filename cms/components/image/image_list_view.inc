<?php

class ImageListView
{
	var $id;
	var $images;
	var $linkFormat;

	function ImageListView($images, $linkFormat, $id = "images")
	{
		$this->id = $id;
		$this->images = $images;
		$this->linkFormat = $linkFormat;
	}
	
	function drawGrid()
	{
		$imgMgr = new ImageManager();
		
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
?>
		<ul class="imageGrid" id="<?echo $this->id?>">
	<?
	
		foreach($this->images as $image)
		{
			$thumb = $imgMgr->thumbnailLink($image->image_id, 125);
			$link = $image->format($this->linkFormat);
			?>
			<li><div class="imageLink"><a href="<?echo $link?>">
			<span class="wrapImage"><img src="<?echo $thumb ?> " alt="Small version of image"/></span>
			<span class="caption"><?echo $image->title?></span></a>
			</div></li>
			<?
	    }
	?>
		</ul>
	<?
	}
}

	
	
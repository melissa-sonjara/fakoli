<?php
Fakoli::usingFeature("facet_manager");

class ImageGridItem
{
	var $grid;
	var $item;
	var $id;
	var $image_id;
	var $title;
	var $text;
	var $onclick;
	var $cssClass = null;
	
	function __construct($grid, $image_id, $title, $text, $onclick = "return false;")
	{
		$this->grid = $grid;
		$this->image_id = $image_id;
		$this->title = $title;
		$this->text = $text;
		$this->onclick = $onclick;
	}
	
	function draw($size, $attrs)
	{
		$imageSize = $this->grid->retinaMode ? $size * 2 : $size;
		$mode = $this->grid->useBackgroundImages ? "thumbnail" : "iconize";
		
		$src = $this->image_id ? "/action/image/{$mode}?image_id={$this->image_id}&size={$imageSize}" : "/fakoli/images/noimage.gif";
		
		$dataAttrs = "";
		foreach($attrs as $name => $value)
		{
			$dataAttrs .= "$name='$value' ";
		}	
		
		$cssClass = $this->cssClass ? "class='{$this->cssClass}' " : "";
		
		if ($this->grid->useBackgroundImages)
		{
			$styles = $this->grid->inlineSizing ? "width: <?echo $size + 10?>px; height: <?echo $size + 40?>px; margin: 10px'" : "";
?>
		<li <?echo $cssClass?>data-details='<?echo $this->id?>' style='background-image: url(<?echo $src?>);background-size: cover center;<?echo $styles?>' onclick='<?echo $this->onclick?>'><div class='title'><?echo $this->title?></div></li>
<?
		}
		else
		{
?>
		<li <?echo $cssClass?>data-details='<?echo $this->id?>' <?echo $dataAttrs?>style='width: <?echo $size + 10?>px; height: <?echo $size + 40?>px; margin: 10px'><div onclick='<?echo $this->onclick?>' style='width: <?echo $size ?>px; height: <?echo $size + 40?>px'><img src="<?echo $src?>" alt="<?echo $this->title?>" style='width: <?echo $size?>px; height: <?echo $size?>px'></img><br/><?echo $this->title?></div></li>
<?
		}
	}
}

class ImageGrid implements FacetFilterable
{
	var $id;
	var $items;
	var $size;
	var $tagItemCallbacks = array();
	var $puff;
	var $useBackgroundImages = false;
	var $retinaMode = false;
	var $cssClass = null;
	var $inlineSizing = true;
	
	function __construct($id, $size = 200, $puff = true, $useBackgroundImages = false)
	{
		$this->id = $id;
		$this->items = array();
		$this->size = $size;
		$this->puff = $puff;
		$this->useBackgroundImages = $useBackgroundImages;
		$this->retinaMode = Settings::getValue("image", "retina_mode");
	}
	
	function getID()
	{
		return $this->id;
	}
	
	function addItem($image_id, $title, $text, $onclick = "return false;")
	{
		$item = new ImageGridItem($this, $image_id, $title, $text, $onclick);
		$this->items[] = $item;
		return $item;
	}
	
	/**
	 * Adds a row tagging handler. This function will receive the data item
	 * for each row and can add custom attributes to the row tag.
	 */
	function addFacetTaggingHandler($handler)
	{
		$this->tagItemCallbacks[] = $handler;
	}
	
	
	function writeScript()
	{
		ob_start();
		
		$puff = $this->puff ? 'true' : 'false';
?>
<script type="text/javascript">
window.addEvent('domready', function() 
{
	new ImageGrid('<?echo $this->id?>', <?echo $this->size?>, {puff: <?echo $puff?>});
});
</script>
<?
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}
	
	function drawView()
	{
		$cssClass = $this->cssClass ? "image_grid $cssClass" : "image_grid";
?>
		<ul id="<?echo $this->id?>" class='<?echo $cssClass?>'>
<?
		foreach($this->items as $item)
		{
			$attrs = array();
			
			foreach($this->tagItemCallbacks as $handler)
			{
				$attrs = call_user_func($handler, $item->item, $attrs);
			}
			 
			$item->draw($this->size, $attrs);
		}
?>
		</ul>		
<?
	}
}
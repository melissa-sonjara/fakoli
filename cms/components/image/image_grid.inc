<?php
Fakoli::usingFeature("facet_manager");

class ImageGridItem
{
	var $item;
	var $id;
	var $image_id;
	var $title;
	var $text;
	var $onclick;
	
	function __construct($image_id, $title, $onclick = "return false;")
	{
		$this->image_id = $image_id;
		$this->title = $title;
		$this->text = $text;
		$this->onclick = $onclick;
	}
	
	function draw($size, $attrs)
	{
		$src = $this->image_id ? "/action/image/fit?image_id={$this->image_id}&size=$size" : "/fakoli/images/noimage.gif";
		
		$dataAttrs = "";
		foreach($attrs as $name => $value)
		{
			$dataAttrs .= "$name='$value' ";
		}		
?>
		<li data-details='<?echo $this->id?>' <?echo $dataAttrs?>style='width: <?echo $size + 10?>px; height: <?echo $size + 40?>px'><div onclick='<?echo $this->onclick?>' style='width: <?echo $size ?>px; height: <?echo $size + 40?>px'><img src="<?echo $src?>" alt="<?echo $this->title?>" style='width: <?echo $size?>px; height: <?echo $size?>px'></img><br/><?echo $this->title?></div></li>
<?
	}
}

class ImageGrid implements FacetFilterable
{
	var $id;
	var $items;
	var $size;
	var $tagItemCallbacks = array();
	
	function __construct($id, $size = 200)
	{
		$this->id = $id;
		$this->items = array();
		$this->size = $size;
	}
	
	function getID()
	{
		return $this->id;
	}
	
	function addItem($image_id, $title, $text)
	{
		$this->items[] = new ImageGridItem($image_id, $title, $text);
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
?>
<script type="text/javascript">
window.addEvent('domready', function() 
{
	new ImageGrid('<?echo $this->id?>', <?echo $this->size?>);
});
</script>
<?
		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}
	
	function drawView()
	{
?>
		<ul id="<?echo $this->id?>" class='zoom_grid'>
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
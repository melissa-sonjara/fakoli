<?php

Fakoli::using("image");

function validURL($item)
{
	return !is_numeric($item->image) && !is_numeric($item->thumbnail);
}
	
/**
 * The Slideshow class provides a reusable user interface control that displays
 * primary source items from the database in a zoomable format. The control is 
 * flexible, and can be used to display multiple images in either a slideshow or
 * lightbox format. Additional information and navigation areas can be shown or 
 * hidden as required.
 *  
 * @author andy
 */
class Slideshow
{
	var $items;
	var $width;
	var $height;
	var $showInfo;
	var $showControls;
	var $showThumbnails;
	var $autoPlay;
	var $preload;
	
	function formatList($field)
	{
		global $config;
		
		$first = true;
		foreach ($this->items as $item)
		{
			if (!$first) $list .= ", ";
			$val = trim($item->$field);
			if ($val[0] == '/') $val = "http://{$config['http_host']}$val";
			
			$list .= "\"".jsSafe($val)."\"";
			$first = false;
		}
		
		echo $list;
	}

	function Slideshow($items, $width=500, $height=400)
	{
		// If we were passed a single item, wrap it in an array
		if (!is_array($items))
		{
			$items = array($items);
		}
		
		$this->items  = array_filter($items, validURL);
		
		$this->width  = $width;
		$this->height = $height;

		$this->showControls = true;
		$this->showInfo = true;
		$this->showThumbnails = true;
		$this->autoPlay = false;
		$this->preload = true;
	}
	
	function add($items)
	{
		if (is_array($items))
		{
			$this->items[] = array_filter($items, validURL);
		}
		else
		{
			if (validURL($items)) $this->items[] = $items;
		}
	}
	
	function writeScript()
	{
		ob_start();
?>
<link rel="stylesheet" type="text/css" href="/cms/components/slideshow/slideshow.css" />
<script type="text/javascript" src="/cms/components/slideshow/slideshow.js"></script>
<!--[if IE]>
<link rel="stylesheet" type="text/css" href="/cms/components/slideshow/ie.css"/>
<![endif]-->

<script type="text/javascript"><!--
/* <![CDATA[ */

window.addEvent('domready', function() {

	var slideshow = new Slideshow({preload: <?echo $this->preload ? "true" : "false"?>, showInfo: <?echo $this->showInfo ? "true" : "false" ?>, autoPlay: <?echo $this->autoPlay ? "true" : "false"?>});

	slideshow.thumbnails = [
<?
	$this->formatList("thumbnail");
?>
	];
	
	slideshow.images = [
<?
	$this->formatList("image_file");
?>
	];
	
	slideshow.captions = [
<?
$first = true;
foreach($this->items as $item)
{
	if (!$first) echo ", ";
?>
					"<?echo jsSafe(trim($item->getCaption())) ?>"
<?
	$first = false;
}
?>	
	];
	
	slideshow.credits = [
<? 
$first = true;
foreach($this->items as $item)
{
	if (!$first) echo ", ";
?>
					"<i><? echo jsSafe(trim($item->getShortCredit())) ?></i>"
<?
	$first = false;
}
?>
	];	

	
	slideshow.urls = [
<?
$first = true;
foreach($this->items as $item)
{
	if (!$first) echo ", ";
?>
					"/primary_sources/item.php?item=<? echo jsSafe(trim($item->LOC_key))?>"
<?	
	$first = false;
	}
?>
	];

	slideshow.keys = [
<?
$first = true;
foreach($this->items as $item)
{
	if (!$first) echo ", ";
?>
					"<? echo jsSafe(trim($item->LOC_key))?>"
<?	
	$first = false;
}
?>
	];

	slideshow.load();
});


//

/* ]]> */
--></script>
<?
		$script = ob_get_contents();
		ob_end_clean();
		
		return $script;
	}

	function writeHTML()
	{
?>
 <div id="progress">
	<div class="text"></div>
	<div class="border"><div class="bar">&nbsp;</div><div id="progress_count"></div></div>
 </div>
<table id="slideshow" cellpadding="0" cellspacing="0" style="width: <? echo $this->width ?>px;">
 <tr>
  <td colspan="3">
    <div id="viewport" style="height: <?echo $this->height ?>px">
	 <div id="gallery" style="height: <?echo $this->height ?>px">
	  <div id="zoom_overlay" style="left: <? echo $this->width - 40 ?>px">
		<table id="zoomer" cellpadding="0" cellspacing="0">
			<tr>
				<td id="zoom_in"></td>
			</tr>
			<tr>
				<td id="zoom_slider">
				<div id="zoom_scale">
				<div id="zoom_thumb"><img src="/cms/components/slideshow/images/zoom_thumb.gif" width="24" height="12" alt="" style="display: block;"/></div>
				</div>
				</td>
			</tr>
			<tr>
				<td id="zoom_out"></td>
			</tr>
		</table>	
	  </div>
	 </div>
	</div>
  </td>
 </tr>
 <tr<? if (!$this->showControls) echo " style='display:none'"; ?>>
  <td colspan="3" id="controlbar"><div id="play_button" class="play">&nbsp;</div><div id="info_button" class="info">&nbsp;</div><span id="caption">&nbsp;</span></td>
 </tr>
 <tr<? if (!$this->showControls) echo " style='display:none'"; ?>>
  <td colspan="3" id="credit">&nbsp;<span id="creditText"></span></td>
 </tr>
</table>
<table>
<tr>
<td width="28%"></td>
<td>
<table id="thumbnails" cellpadding="0" cellspacing="0" style="width:<? echo $this->width; if (!$this->showThumbnails) echo "; display:none"; ?>">
 <tr>
  <td id="scroll_left"></td>
  <td id="strip">
   <div id="thumbnail_strip" style="width: <? echo $this->width - 46 ?>">
 	<div id="thumbnail_scroll"><div id="thumbnail_highlight"></div></div>
 	</div>
  </td>
  <td id="scroll_right"></td>
 </tr>
</table>
</td>
</tr>
</table>
<?
	}
}
?>
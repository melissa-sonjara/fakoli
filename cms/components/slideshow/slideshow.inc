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

Fakoli::using("image");
	
/**
 * The Slideshow class provides a reusable user interface control that displays
 * images from the database in a zoomable format. The control is 
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

	function Slideshow($items, $width=500, $height=400)
	{
		// If we were passed a single item, wrap it in an array
		if (!is_array($items))
		{
			$items = array($items);
		}
		
		$this->items = $items;
		
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
		$mgr = new ImageManager();
		global $config;
		
		ob_start();
?>
<script type="text/javascript"><!--
/* <![CDATA[ */

window.addEvent('domready', function() {

	var slideshow = new Slideshow({preload: <?echo $this->preload ? "true" : "false"?>, showThumbnails: <?echo $this->showThumbnails ? "true" : "false"?>, showInfo: <?echo $this->showInfo ? "true" : "false" ?>, autoPlay: <?echo $this->autoPlay ? "true" : "false"?>});

	slideshow.thumbnails = [
<?
	$thumbs = array();
	foreach($this->items as $item)
	{
		$thumbs[] = "'http://".$config['http_host'].$mgr->thumbnailLink($item->image_id, 100)."'";
	}
	
	echo implode(", ", $thumbs);
?>
	];
	
	slideshow.images = [
<?
	$images = array();
	foreach($this->items as $item)
	{
		$images[] = "'http://".$config['http_host'].$mgr->imageLink($item->image_id)."'";
	}

	echo implode(", ", $images);
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
				<div id="zoom_thumb"><img src="/fakoli/slideshow/images/zoom_thumb.gif" width="24" height="12" alt="" style="display: block;"/></div>
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
<table cellpadding="0" cellspacing="0">
<tr>
<td>
<table id="thumbnails" cellpadding="0" cellspacing="0" style="width:<? echo $this->width."px"; if (!$this->showThumbnails) echo "; display:none"; ?>">
 <tr>
  <td id="scroll_left"></td>
  <td id="strip">
   <div id="thumbnail_strip" style="width: <? echo $this->width - 46 ?>px">
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
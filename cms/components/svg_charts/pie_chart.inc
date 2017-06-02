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

Fakoli::using("svg_charts");

class PieChart
{
	var $width;
	var $height;
	var $containerWidth;
	var $containerHeight;
	var $cx;
	var $cy;
	var $values;
	var $labels;
	var $palette;
	var $labelSize = 12;
	var $strokeWidth = 1;
	var $animate = true;
	var $shadow = true;
	var $emboss = false;
	var $legend = false;
	var $legendX = 0;
	var $legendY = 0;
	var $legendSwatchSize = 20;
	var $legendLineHeight = 30;
	var $percentages = true;
	var $percentagesSize = 16;
	var $percentagesDistance = 0.75;
	var $onSectorOver = "null";
	var $onSectorOut = "null";
	var $onSectorClick = "null";
	var $onLegendOver = "null";
	var $onLegendOut = "null";
	var $scaleToFit = false;
	var $fontFamily = 'Arial, Helvetica';
	var $canvasBackground = "#FFFFFF";
	var $selectable = false;
	var $animateSelection = false;
	
	function PieChart($id, $width = 300, $height = 300, $values = null, $labels = null, $palette = "standard")
	{
		$this->id = $id;
		$this->setSize($width, $height);
		$this->setContainerSize($width, $height);
		$this->values = $values;
		$this->labels = $labels;
		$this->palette = $palette;
	}
	
	function setSize($width, $height)
	{
		$this->width = $width;
		$this->height = $height;
		if (!$this->cx) $this->cx = $this->width / 2;
		if (!$this->cy) $this->cy = $this->height / 2;
		if (!$this->radius) $this->radius = min($width, $height) * .4;
	}
	
	function setContainerSize($width, $height = "0")
	{
		$this->containerWidth = $width;
		$this->containerHeight = $height;
	}
	
	function setCenter($cx, $cy)
	{
		$this->cx = $cx;
		$this->cy = $cy;
	}
	
	function setRadius($r)
	{
		$this->radius = $r;
	}
	
	function setLabelSize($size)
	{
		$this->labelSize = $size;
	}
	
	function setStrokeWidth($s)
	{
		$this->strokeWidth = $s;
	}
	
	function setFontFamily($font)
	{
		$this->fontFamily = $font;
	}
	
	function showLegend($show, $x = 0, $y = 0)
	{
		$this->legend = $show;
		$this->legendX = $x;
		$this->legendY = $y;
	}
	
	function showPercentages($show, $size = 16, $distance = 0.75)
	{
		$this->percentages = $show;
		$this->percentagesSize = $size;
		$this->percentagesDistance = $distance;
	}
	
	function draw()
	{
		global $script;
		$values = implode(", ", $this->values);
		$labels = "'".implode("', '", $this->labels) . "'";
		$animate = $this->animate ? "true" : "false";
		$shadow = $this->shadow ? "true": "false";	
		$emboss = $this->emboss ? "true" : "false";
		$legend = $this->legend ? "true" : "false";
		$percentages = $this->showPercentages ? "true" : "false";
		$download = $this->enableDownload ? "true" : "false";
		$selectable = $this->selectable ? "true": "false";
		$animateSelection = $this->animateSelection ? "true" : "false";
		
		$containerWidth = is_numeric($this->containerWidth) ? $this->containerWidth . "px" : $this->containerWidth;
		$containerHeight = is_numeric($this->containerHeight) ? $this->containerHeight . "px" : $this->containerHeight;
		
		$script .= <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function ()
{
	var {$this->id} = new PieChart('{$this->id}', 
	{
		width: {$this->width}, 
		height: {$this->height}, 
		cx: {$this->cx}, 
		cy: {$this->cy}, 
		radius: {$this->radius}, 
		palette: '{$this->palette}',
		labelSize: {$this->labelSize},
		fontFamily: '{$this->fontFamily}',
		strokeWidth: {$this->strokeWidth},
		animate: {$animate},
		shadow: {$shadow},
		emboss: {$emboss},
		legend: {$legend},
		legendX: {$this->legendX},
		legendY: {$this->legendY},
		legendSwatchSize: {$this->legendSwatchSize},
		legendLineHeight: {$this->legendLineHeight},
		percentages: {$percentages},
		percentagesSize: {$this->percentagesSize},
		onSectorOver: {$this->onSectorOver},
		onSectorOut: {$this->onSectorOut},
		onSectorClick: {$this->onSectorClick},
		onLegendOver: {$this->onLegendOver},
		onLegendOut: {$this->onLegendOut},
		enableDownload: {$download},
		canvasBackground: '{$this->canvasBackground}',
		selectable: {$selectable},
		animateSelection: {$animateSelection}
	});
	{$this->id}.values = [ $values ];
	{$this->id}.labels = [ $labels ];
	
	{$this->id}.draw();
});
</script>
ENDSCRIPT;

	if ($this->scaleToFit)
	{
		$dimensions = "width: 100%; height: auto";
	}
	else
	{
		$dimensions = "width: {$containerWidth}; height: {$containerHeight};";
	}
	
		echo "<div id='{$this->id}' style='$dimensions'></div>";
	}	
}
?>
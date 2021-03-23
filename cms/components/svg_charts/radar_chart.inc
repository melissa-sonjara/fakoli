<?php
/**\addtogroup svg_charts
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

Fakoli::using("svg_charts");

class RadarChart
{
	var $width;
	var $height;
	var $containerWidth;
	var $containerHeight;
	var $cx;
	var $cy;
	var $max = 0;
	var $min = 0;
	var $radius;
	var $values;
	var $labels;
	var $palette;
	var $ticks = 10;
	var $labelSize = 12;
	var $strokeWidth = 1;
	var $animate = true;
	var $shadow = true;
	var $emboss = false;
	var $legend = false;
	var $legendX = 0;
	var $legendY = 0;
	var $titleSize = 20;
	var $legendSwatchSize = 20;
	var $legendLineHeight = 30;
	var $onLegendOver = "null";
	var $onLegendOut = "null";
	var $onLegendClick = "null";
	var $scaleToFit = false;
	var $fontFamily = 'Arial, Helvetica';

	function __construct($id, $width = 300, $height = 300, $labels = null, $palette = "standard")
	{
		$this->id = $id;
		$this->setSize($width, $height);
		$this->setContainerSize($width, $height);
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

	function setLabels($labels)
	{
		$this->labels = $labels;
	}
	function showLegend($show, $x = 0, $y = 0)
	{
		$this->legend = $show;
		$this->legendX = $x;
		$this->legendY = $y;
	}

	function setPalette($palette)
	{
		$this->palette = $palette;
	}
	
	function addSeries($series)
	{
		$this->series[] = $series;
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

		$containerWidth = is_numeric($this->containerWidth) ? $this->containerWidth . "px" : $this->containerWidth;
		$containerHeight = is_numeric($this->containerHeight) ? $this->containerHeight . "px" : $this->containerHeight;

		$data = "";
		
		foreach($this->series as $s)
		{
			$data .= "\t{$this->id}.addSeries(".$s->toScript().");\n";
		}
		
		$script .= <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function ()
{
	var {$this->id} = new RadarChart('{$this->id}',
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
		ticks: {$this->ticks},
		max: {$this->max},
		min: {$this->min},
		animate: {$animate},
		shadow: {$shadow},
		emboss: {$emboss},
		legend: {$legend},
		legendX: {$this->legendX},
		legendY: {$this->legendY},
		legendSwatchSize: {$this->legendSwatchSize},
		legendLineHeight: {$this->legendLineHeight},
		onLegendOver: {$this->onLegendOver},
		onLegendOut: {$this->onLegendOut},
		onLegendClick: {$this->onLegendClick},
		title: '{$this->title}',
		titleSize: {$this->titleSize},
		enableDownload: {$download},
		units: '{$this->units}'
	});
	
	{$this->id}.labels = [ $labels ];
{$data}
	
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
/**@}*/?>
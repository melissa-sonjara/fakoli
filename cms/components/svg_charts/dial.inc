<?php
/**************************************************************

Copyright (c) 2016 Sonjara, Inc

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

class Dial
{
	var $width;
	var $height;
	var $containerWidth;
	var $containerHeight;
	var $cx;
	var $cy;
	var $radius = 0;
	var $value = 0;
	var $label = "";
	var $palette;
	var $labelSize = 12;
	var $labelX;
	var $labelY;
	var $strokeWidth = 1;
	var $animate = true;
	var $shadow = true;
	var $scaleToFit = false;
	var $fontFamily = 'Arial, Helvetica';
	var $min = 0;
	var $max = 100;
	var $minColor = '#f00';
	var $maxColor = '#0f0';
	var $colorMode = 'fixed';
	var $showTarget = false;
	var $target = 0;
	var $targetLabel = "";
	var $cssClass = null;
	
	function __construct($id, $width = 300, $height = 300, $value = 0, $label = '', $palette = "standard")
	{
		$this->id = $id;
		$this->setSize($width, $height);
		$this->setContainerSize($width, $height);
		$this->value = $value;
		$this->label = $label;
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

	function setLabelPosition($lx, $ly)
	{
		$this->labelX = $lx;
		$this->labelY = $ly;
	}
	
	function setRange($min, $max)
	{
		$this->min = $min;
		$this->max = $max;
	}
	
	function setColorRange($min, $max = null)
	{
		if (!$max) 
		{
			$max = $min;
		}
		
		if ($max == $min)
		{
			$this->colorMode = 'fixed';
		}
		else
		{
			$this->colorMode = 'interpolated';
		}
		
		$this->minColor = $min;
		$this->maxColor = $max;
	}
	
	function setStrokeWidth($s)
	{
		$this->strokeWidth = $s;
	}

	function setFontFamily($font)
	{
		$this->fontFamily = $font;
	}

	function showTarget($target, $targetLabel)
	{
		$this->showTarget = true;
		$this->target = $target;
		$this->targetLabel = $targetLabel;
	}
	
	function draw()
	{
		global $script;
		$animate = $this->animate ? "true" : "false";
		$shadow = $this->shadow ? "true": "false";
		$legend = $this->legend ? "true" : "false";
		$percentages = $this->showPercentages ? "true" : "false";
		$download = $this->enableDownload ? "true" : "false";
		$showTarget = $this->showTarget ? "true" : "false";
		$targetLabel = jsSafe($this->targetLabel);
		$target = ($this->target == '') ? 0 : $this->target;
		
		$containerWidth = is_numeric($this->containerWidth) ? $this->containerWidth . "px" : $this->containerWidth;
		$containerHeight = is_numeric($this->containerHeight) ? $this->containerHeight . "px" : $this->containerHeight;

		$script .= <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function ()
{
	var {$this->id} = new Dial('{$this->id}', $this->value, '{$this->label}', 
	{
		width: {$this->width},
		height: {$this->height},
		cx: {$this->cx},
		cy: {$this->cy},
		radius: {$this->radius},
		palette: '{$this->palette}',
		labelSize: {$this->labelSize},
		labelx: {$this->labelX},
		labely: {$this->labelY},
		fontFamily: '{$this->fontFamily}',
		strokeWidth: {$this->strokeWidth},
		animate: {$animate},
		shadow: {$shadow},
		enableDownload: {$download},
		min: {$this->min},
		max: {$this->max},
		minColor: '{$this->minColor}',
		maxColor: '{$this->maxColor}',
		colorMode: '{$this->colorMode}',
		showTarget: {$showTarget},
		target: {$target},
		targetLabel: '{$targetLabel}'
	});

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

	$cssClass = ($this->cssClass) ? " class='{$this->cssClass}'" : "";
	echo "<div id='{$this->id}'{$cssClass} style='$dimensions'></div>";
	}
}
?>
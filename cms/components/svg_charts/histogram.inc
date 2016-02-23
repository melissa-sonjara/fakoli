<?php

class HistogramSeries
{
	var $type;
	var $title;
	var $values;
	var $options = array();
	var $onClick;
	var $tooltips = array();
	
	function __construct($type, $title, $values)
	{
		$this->type = $type;
		$this->title = $title;
		$this->values = $values;
	}
	
	function set($attr, $val)
	{
		$this->options[$attr] = $val;
		return $this;
	}
	
	function toScript()
	{
		$v = array();
		
		foreach($this->values as $value)
		{
			if ($value === "" || !isset($value)) $value = "null";
			$v[] = $value;
		}
		 
		$values = implode(", ", $v);
		$options = array();
		foreach ($this->options as $opt => $val)
		{
			if ($val === false) $val = "false";
			if ($val === true) $val = "true";
			
			$options[] = "'$opt': $val";
		}

		if ($this->onClick)
		{
			$options[] = "'onClick': {$this->onClick} ";
		} 

		if (count($this->tooltips))
		{
			$tooltips = "['".implode("', '", array_map(jsSafe, $this->tooltips))."']";
			$options[] = "'toolTips': $tooltips";
		}
		
		$options = "{".implode(", ", $options)."}";
		
			
		$out = "new HistogramSeries('{$this->type}', '" . jsSafe($this->title) . "', [{$values}], $options)";
		return $out;
	}
}

class GroupedHistogramSeries
{
	var $type;
	var $title;
	var $children = array();

	function __construct($type, $title)
	{
		$this->type = $type;
		$this->title = $title;
	}

	function addSeries($series)
	{
		$this->children[] = $series;
	}
	
	function toScript()
	{
		$out = "(new GroupedHistogramSeries('{$this->type}', '". jsSafe($this->title) ."'))";
		foreach($this->children as $child)
		{
			$kid = $child->toScript();
			$out .= ".addSeries({$kid})";
		}
		
		return $out;
	}
}

class Histogram
{
	var $width;
	var $height;
	var $orientation = "vertical";
	var $cssStyles;
	var $containerWidth;
	var $containerHeight;
	var $chartLeft;
	var $chartTop;
	var $chartWidth;
	var $chartHeight;
	var $chartBackground = "#ddd";
	var $palette;
	var $labelSize = 12;
	var $strokeWidth = 1;
	var $animate = false;
	var $shadow = true;
	var $emboss = false;
	var $legend = false;
	var $legendX = 0;
	var $legendY = 0;
	var $legendSwatchSize = 20;
	var $legendLineHeight = 30;
	var $xAxisTitle = "";
	var $yAxisTitle = "";
	var $series = array();
	var $ticks = 10;
	var $max = 0;
	var $min = 0;
	var $columnMargin = 0.2;
	var $morphing = false;
	var $startIndex = 0;
	var $yAxisLabels = array();
	var $title = "";
	var $titleSize = 20;
	var $enableDownload = true;
	var $units = "";
	var $scaleToFit = false;
	
	function __construct($id, $width = 300, $height = 300, $chartLeft = 25, $chartTop = 25, $chartWidth = 250, $chartHeight = 250, $palette = "standard")
	{
		$this->id = $id;
		$this->setSize($width, $height);
		$this->setContainerSize($width, $height);
		$this->positionChart($chartLeft, $chartTop, $chartWidth, $chartHeight);
		
		$this->palette = $palette;
	}
	
	function setSize($width, $height)
	{
		$this->width = $width;
		$this->height = $height;
	}

	function setContainerSize($width, $height = "0")
	{
		$this->containerWidth = $width;
		$this->containerHeight = $height;
	}
	
	function positionChart($left, $top, $width, $height)
	{
		$this->chartLeft = $left;
		$this->chartTop = $top;
		$this->chartWidth = $width;
		$this->chartHeight = $height;
	}
	
	function setLabels($labels)
	{
		$this->labels = $labels;
	}
	
	function setLabelSize($size)
	{
		$this->labelSize = $size;
	}
	
	function setYAxisLabels($labels)
	{
		$this->yAxisLabels = $labels;
	}
	
	function setStrokeWidth($s)
	{
		$this->strokeWidth = $s;
	}
	
	function setPalette($palette)
	{
		$this->palette = $palette;
	}
	
	function showLegend($show, $x = 0, $y = 0)
	{
		$this->legend = $show;
		$this->legendX = $x;
		$this->legendY = $y;
	}
		
	function addSeries($series)
	{
		$this->series[] = $series;
	}
	
	function autoScale()
	{
		$max = 0;
		$min = 0;
		
		foreach($this->series as $series)
		{
			foreach($series->values as $value)
			{
				if ($max === null || $value > $max) $max = $value;
				if ($min === null || $value < $min) $min = $value;
			}
		}
		
		$logMax = intval(log10($max));
		$logMin = intval(log10($min));
		$log = max($logMax, $logMin);
		
		$denom = pow(10, $log);
		
		if ($max != 0)
		{
			$max = floor(($max / $denom) + 1) * $denom; 
		}
		
		if ($min != 0)
		{
			$min = floor(($min / $denom) - 1) * $denom; 
		}
		
		$this->max = $max;
		$this->min = $min;
	}
		
	function draw()
	{
		global $script;
		
		$labels = "'".implode("', '", $this->labels) . "'";
		
		if (count($this->yAxisLabels))
		{
			$yaxis = "'".implode("', '", $this->yAxisLabels) . "'";
			$yaxis = "\t{$this->id}.yAxisLabels = [ $yaxis ];";
		}
		
		$animate = $this->animate ? "true" : "false";
		$shadow = $this->shadow ? "true": "false";	
		$emboss = $this->emboss? "true" : "false";
		$legend = $this->legend ? "true" : "false";
		$enableDownload = $this->enableDownload ? "true" : "false";
		
		$containerWidth = is_numeric($this->containerWidth) ? $this->containerWidth . "px" : $this->containerWidth;
		$containerHeight = is_numeric($this->containerHeight) ? $this->containerHeight . "px" : $this->containerHeight;
		
		$data = "";
		
		foreach($this->series as $s)
		{
			$data .= "\t{$this->id}.addSeries(".$s->toScript().");\n";
		}
		
		$jsClass = $this->morphing ? "MorphingHistogram" : "Histogram";
		
		$script .= <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function ()
{
	var {$this->id} = new $jsClass('{$this->id}', 
	{
		width: {$this->width}, 
		height: {$this->height}, 
		orientation: '{$this->orientation}',
		chartLeft: {$this->chartLeft}, 
		chartTop: {$this->chartTop}, 
		chartWidth: {$this->chartWidth},
		chartHeight: {$this->chartHeight},
		chartBackground: '{$this->chartBackground}',
		palette: '{$this->palette}',
		labelSize: {$this->labelSize},
		strokeWidth: {$this->strokeWidth},
		animate: {$animate},
		shadow: {$shadow},
		emboss: {$emboss},
		legend: {$legend},
		legendX: {$this->legendX},
		legendY: {$this->legendY},
		legendSwatchSize: {$this->legendSwatchSize},
		legendLineHeight: {$this->legendLineHeight},
		ticks: {$this->ticks},
		max: {$this->max},
		min: {$this->min},
		startIndex: {$this->startIndex},
		columnMargin: {$this->columnMargin},
		title: '{$this->title}',
		titleSize: {$this->titleSize},
		enableDownload: {$enableDownload},
		units: '{$this->units}'
	});
	
	{$this->id}.labels = [ {$labels} ];
{$yaxis}
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
		
		echo "<div id='{$this->id}' style='{$this->cssStyles}'></div>";
	}	
}
?>
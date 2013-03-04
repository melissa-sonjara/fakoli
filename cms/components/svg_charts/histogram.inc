<?php

class HistogramSeries
{
	var $type;
	var $title;
	var $values;
	var $options = array();
	var $onClick;
	var $tooltips = array();
	
	function HistogramSeries($type, $title, $values)
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

class Histogram
{
	var $width;
	var $height;
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
	var $enabledDownload = true;
	
	function Histogram($id, $width = 300, $height = 300, $chartLeft = 25, $chartTop = 25, $chartWidth = 250, $chartHeight = 250, $palette = "standard")
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
		enableDownload: {$enableDownload}
	});
	
	{$this->id}.labels = [ {$labels} ];
{$yaxis}
{$data}
	{$this->id}.draw();
});
</script>
ENDSCRIPT;
		echo "<div id='{$this->id}' style='width: {$containerWidth}; height: {$containerHeight};{$this->cssStyles}'></div>";
	}	
}
?>
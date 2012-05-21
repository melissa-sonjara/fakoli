<?php

class HistogramSeries
{
	var $type;
	var $title;
	var $values;
	var $options = array();
	
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
		$values = implode(", ", $this->values);
		$options = array();
		foreach ($this->options as $opt => $val)
		{
			$options[] = "'$opt': '$val',";
		}
		
		$options = "{".implode(",", $options)."}";
		
		$out = "new HistogramSeries('{$this->type}', '" . jsSafe($this->title) . "', [{$values}], $options)";
		return $out;
	}
}

class Histogram
{
	var $width;
	var $height;
	var $chartLeft;
	var $chartTop;
	var $chartWidth;
	var $chartHeight;
	var $palette;
	var $labelSize = 12;
	var $strokeWidth = 1;
	var $animate = false;
	var $shadow = true;
	var $emboss = false;
	var $legend = false;
	var $legendX = 0;
	var $legendY = 0;
	var $xAxisTitle = "";
	var $yAxisTitle = "";
	var $series = array();
	var $ticks = 10;
	var $max = 0;
	var $morphing = false;
	
	function Histogram($id, $width = 300, $height = 300, $chartLeft = 25, $chartTop = 25, $chartWidth = 250, $chartHeight = 250, $palette = "standard")
	{
		$this->id = $id;
		$this->setSize($width, $height);
		$this->positionChart($chartLeft, $chartTop, $chartWidth, $chartHeight);
		
		$this->palette = $palette;
	}
	
	function setSize($width, $height)
	{
		$this->width = $width;
		$this->height = $height;
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
	
	function setStrokeWidth($s)
	{
		$this->strokeWidth = $s;
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
		$animate = $this->animate ? "true" : "false";
		$shadow = $this->shadow ? "true": "false";	
		$legend = $this->legend ? "true" : "false";
		
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
		palette: '{$this->palette}',
		labelSize: {$this->labelSize},
		strokeWidth: {$this->strokeWidth},
		animate: {$animate},
		shadow: {$shadow},
		legend: {$legend},
		legendX: {$this->legendX},
		legendY: {$this->legendY},
		ticks: {$this->ticks},
		max: {$this->max}
	});
	
	{$this->id}.labels = [ {$labels} ];
{$data}
	{$this->id}.draw();
});
</script>
ENDSCRIPT;
		echo "<div id='{$this->id}' style='width: {$this->width}px; height {$this->height}px'></div>";
	}	
}
?>
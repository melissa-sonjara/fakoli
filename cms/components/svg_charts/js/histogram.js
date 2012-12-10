var HistogramSeries = new Class(
{
	Implements: [Options, Events],
	type: 'block',
	title: '',
	values: [],
	max: 0,
	min: 0,
	options:
	{
		shadow: false,
		emboss: true,
		styles: {},
		strokeWidth: 2,
		symbolSize: 4,
		onClick: Class.Empty,
		onMouseOver: Class.Empty,
		onMouseOut: Class.Empty,
		colorMode: 'series',
		toolTips: [],
		indicateTooltips: false,
		areaFill: false,
		areaFillOpacity: 0
	},
	columns: [],
	renderer: Class.Empty,
	chart: Class.Empty,
	
	initialize: function(type, title, values, options)
	{
		this.type = type;
		this.title = title;
		this.values = values;
		this.max = this.values.max();
		this.min = this.values.min();
		
		this.setOptions(options);
	},
	
	draw: function(chart, index)
	{
		if (!this.renderer)
		{
			switch(this.type)
			{
			case 'block':
				
				this.renderer = new BlockSeriesRenderer(chart, this, index);
				break;
				
			case 'line':
				
				this.renderer = new LineSeriesRenderer(chart, this, index);
				break;
			}
			
			if (!this.renderer) return;
		}
		this.renderer.draw();
	},
	
	drawFill: function(chart, index)
	{
		if (!this.renderer)
		{
			switch(this.type)
			{
			case 'block':
				
				this.renderer = new BlockSeriesRenderer(chart, this, index);
				break;
				
			case 'line':
				
				this.renderer = new LineSeriesRenderer(chart, this, index);
				break;
			}
			
			if (!this.renderer) return;
		}
		this.renderer.drawFill();
	},
	
	morph: function(series)
	{
		if (!this.renderer) return;
		this.renderer.morph(series);		
	},
	
	hasTooltip: function(idx)
	{
		if (!this.options.toolTips) return false;
		
		if (idx > this.options.toolTips.length) return false;
		
		var text = this.options.toolTips[idx];
		if (text == '') return false;
		
		return true;
	},
	
	showToolTip: function(evt, idx)
	{
		if (idx > this.options.toolTips.length) return;
		
		var text = this.options.toolTips[idx];
		if (text == '') return;
		
		showTextToolTip(this.chart.container, evt, this.chart.id + "_tooltip", this.options.toolTips[idx]);		
	},
	
	hideToolTip: function()
	{
		hideToolTip(this.chart.id + "_tooltip");
	}	
});

var BlockSeriesRenderer = new Class(
{
	chart: Class.Empty,
	series: Class.Empty,
	index: 0,
	
	initialize: function(chart, series, index)
	{
		this.chart = chart;
		this.series = series;
		this.index = index;
	},
	
	draw: function()
	{
		this.series.values.each(function(val, i)
		{
			var fillSwatch = this.chart.palette.swatches[(this.series.options.colorMode == 'series') ? this.index : i];			

			var columnWidth = this.chart.blockWidth;

			var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth) + (this.chart.blockSeriesDrawn * columnWidth);
			var columnLeft = this.chart.columnWidth * i + columnOffset;
			
			var x = this.chart.options.chartLeft + columnLeft;
			var columnHeight = this.chart.options.chartHeight * val / this.chart.range();
			var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight - this.chart.xAxisOffset;
			
			var column = this.chart.paper.rect(x, y, columnWidth, columnHeight);
			column.attr({fill: fillSwatch});
			
			if (this.series.options.emboss)
			{
				column.emboss();
			}

			if (this.series.options.shadow)
			{
				column.dropShadow(5, 1, 1, 0.1);
			}
			
			column.mouseover(function(e) { this.series.fireEvent('mouseOver', [e, i]); }.bind(this));
			column.mouseout(function(e) { this.series.fireEvent('mouseOut', [e, i]); }.bind(this));
			
			this.series.columns.push(column);
			
		}.bind(this));

		this.chart.blockSeriesDrawn++;
	},
	
	drawFill: function()
	{
	},
	
	// Morph this series to match the values of the supplied series
	morph: function(series)
	{
		series.values.each(function(val, i)
		{
			var columnHeight = this.chart.options.chartHeight * val / this.chart.range() + this.chart.xAxisOffset;
			var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight;

			this.series.columns[i].animate({'y' :y, 'height': columnHeight}, 1000, "<>");
		}.bind(this));
	}
});

var LineSeriesRenderer = new Class(
{
	chart: Class.Empty,
	series: Class.Empty,
	index: 0,
	path: Class.Empty,
	dots: [],
	
	initialize: function(chart, series, index)
	{
		this.chart = chart;
		this.series = series;
		this.index = index;
	},
	
	draw: function()
	{
		var lineColor = this.chart.palette.swatches[this.index];
		var fillColor = this.chart.options.chartBackground;
		var p = this.calculatePath(this.series, false);

		
		this.path = this.chart.paper.path(p).attr({"stroke-width": this.series.options.strokeWidth, stroke: lineColor});
		
		this.coords.each(function(c, i) {
			var dot = this.chart.paper.circle(c.x, c.y, this.series.options.symbolSize).attr({"stroke-width": this.series.options.strokeWidth, stroke: lineColor, fill: (this.series.options.indicateTooltips && this.series.hasTooltip(i) ) ? lineColor : fillColor, 'cursor': 'pointer'});
			dot.mouseover(function(e) {dot.animate({'r': this.series.options.symbolSize * 2}, 250, "backout"); this.series.fireEvent('mouseOver', [e, i]); this.series.showToolTip(e, i);}.bind(this));
			dot.mouseout(function(e) {dot.animate({'r': this.series.options.symbolSize}, 250, "backout");  this.series.fireEvent('mouseOut', [e, i]); this.series.hideToolTip();}.bind(this));
			dot.click(function() { this.series.fireEvent('click', i); }.bind(this));
			this.dots.push(dot);
		}.bind(this));	
	},
	
	drawFill: function()
	{
		var lineColor = this.chart.palette.swatches[this.index];
		var fillColor = this.chart.palette.swatches[this.index];
		
		if (this.series.options.areaFill)
		{
			var f = this.calculatePath(this.series, true);
			this.fill = this.chart.paper.path(f).attr({"stroke-width": 0, stroke: lineColor, fill: lineColor, 'fill-opacity': this.series.options.areaFillOpacity});
		}
	},
	
	calculatePath: function(series, closed)
	{
		var p = "";
		var cmd = "M";
		this.coords = [];
		var startX = 0;
		
		series.values.each(function(val, i)
		{
			var columnWidth = this.chart.blockWidth;

			var columnOffset = this.chart.columnWidth / 2;
			var columnCenter = this.chart.columnWidth * i + columnOffset;
			
			
			var x = this.chart.options.chartLeft + columnCenter;
			var columnHeight = this.chart.options.chartHeight * val / this.chart.range() + this.chart.xAxisOffset;
			var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight;
			
			this.coords.push({'x': x, 'y': y});
			p += cmd + x + "," + y;
			cmd = "L";			
			
			if (i == 0) startX = x;
			
			if (i == series.values.length - 1 && closed)
			{
				y = this.chart.options.chartTop + this.chart.options.chartHeight - this.chart.xAxisOffset;
				p += "L" + x + "," + y + "L" + startX + "," + y + "Z";
			}
			
		}.bind(this));
		
		return p;
	},
	
	// Morph this series to match the values of the supplied series
	morph: function(series)
	{
		var p = this.calculatePath(series);
		var lineColor = this.chart.palette.swatches[series.index];
		var fillColor = this.chart.options.chartBackground;
		this.path.animate({'path': p, 'stroke': lineColor}, 1000, "<>");
		
		this.dots.each(function(dot, i) 
		{
			dot.animate({'cy': this.coords[i].y, 'stroke': lineColor, fill: (this.series.options.indicateTooltips && this.series.hasTooltip(i) ) ? lineColor : fillColor}, 1000, "<>");
		}.bind(this));
	}		
});

var Histogram = new Class(
{
	Extends: Chart,
	Implements: [Options],
	series: [],
	labels: [],
	yAxisLabels: [],
	max: 0,
	min: 0,
	xAxisOffset: 0,
	
	options:
	{
		palette: 'standard',
		width: 600,
		height: 400,
		chartLeft: 50,
		chartTop: 50,
		chartWidth: 500,
		chartHeight: 300,
		chartBackground: "#ddd",
		labelSize: 12,
		strokeWidth: 2,
		animate: true,
		shadow: false,
		gridStrokeWidth: 1,
		ticks: 10,
		columnMargin: 0.2,
		titleSize: 20,
		title: '',
		max: 0,
		min: 0,
		enableDownload: true
	},
	
	initialize: function(id, options, labels)
	{
		this.parent(id);
		this.setOptions(options);
		this.labels = labels;
	},
	
	addSeries: function(series)
	{
		this.series.push(series);
		series.chart = this;
		series.index = this.series.length - 1;
	},
	
	updateSeries: function(index, values)
	{
		this.series[index].values = values;
		this.series[index].morph(this.series[index]);
	},
	
	updateTooltips: function(index, tips)
	{
		this.series[index].options.tooltips = tips;
	},
	
	range: function()
	{
		return this.max - this.min;
	},
	
	drawChart: function()
	{
		this.createChart();

		this.setupHistogram();
		
		this.blockWidth = (this.columnWidth / this.blockSeriesCount) * (1 - this.options.columnMargin);
		this.drawHistogram();
	},
	
	drawHistogram: function()
	{

		this.drawBlocks();
		this.drawLabels();
		this.drawTicks();
		this.drawLegend();
		this.drawTitle();
	},
	
	setupHistogram: function()
	{
		if (this.labels.length == 0)
		{
			count = this.series[0].length;
			for(i = 1; i <= count; ++i)
			{
				this.labels.push(i);
			}
		}
		
		this.max = this.options.max;
		this.min = this.options.min;
		
		this.series.each(function(s)
		{
			if (s.max > this.max) this.max = s.max;
			if (s.min < this.min) this.min = s.min;
		}.bind(this));
		
		this.max = Math.ceil(this.max / this.options.ticks) * this.options.ticks;
		
		if (this.min < 0) 
		{
			this.xAxisOffset = -(this.options.chartHeight * this.min / this.range());
		}
		
		this.blockSeriesCount = 0;
		this.blockSeriesDrawn = 0;
		
		this.series.each(function(s)
		{
			if (s.type == 'block')
			{
				this.blockSeriesCount++;
			}
		}.bind(this));	
	
		var grid = this.paper.rect(this.options.chartLeft, this.options.chartTop, this.options.chartWidth, this.options.chartHeight);
		grid.attr({"stroke-width": this.options.strokeWidth, stroke: this.palette.strokeColor, fill: this.options.chartBackground});
		if (this.options.shadow)
		{
			grid.dropShadow(7, 1, 1, 0.2);
		}
		
		var count = this.labels.length;
		
		this.columnWidth = this.options.chartWidth / count;
		
		var grid = this.drawGrid(this.options.chartLeft, this.options.chartTop, this.options.chartWidth, this.options.chartHeight, this.labels.length, this.options.ticks, this.palette.strokeColor);
	},
	
	drawBlocks: function()
	{
		var idx = 0;
		this.series.each(function(s)
		{
			s.drawFill(this, idx++);
		}.bind(this));	
		
		idx = 0;
		this.series.each(function(s)
		{
			s.draw(this, idx++);
		}.bind(this));		
	},
	
	drawLabels: function()
	{
		this.labels.each(function(text, index)
		{
			var x = this.options.chartLeft + this.columnWidth * index + (this.columnWidth / 2);
			var y = this.options.chartTop + this.options.chartHeight + 20 + (text.count("\n") * this.options.labelSize / 2);
			
			var label = this.paper.text(x, y, text);
			label.attr({stroke: 'none', fill: this.palette.strokeColor, "font-size": this.options.labelSize});
		}.bind(this));
	},
	
	drawTicks: function()
	{
		var increment = this.range() / this.options.ticks;
		
		if (increment == 0) return;
		
		var tick = this.min;
		var idx = 0;
		var y = this.options.chartTop + this.options.chartHeight;
		var ystep = this.options.chartHeight / this.options.ticks;
		
		for(tick = this.min; tick <= this.max; tick += increment)
		{
			if (this.yAxisLabels.length > 0)
			{
				label = this.yAxisLabels[idx];
			}
			else
			{
				label = number_format(tick, 0);
			}
			var text = this.paper.text(this.options.chartLeft - 10, y, label);
			text.attr({stroke: 'none', fill: this.palette.strokeColor, "font-size": this.options.labelSize, "text-anchor": "end"});
			y -= ystep;
			++idx;
		}
	},
	
	drawGrid: function(x, y, w, h, wv, hv, color) 
	{
	    color = color || "#000";
	    var path = ["M", Math.round(x) + .5, Math.round(y) + .5, "L", Math.round(x + w) + .5, Math.round(y) + .5, Math.round(x + w) + .5, Math.round(y + h) + .5, Math.round(x) + .5, Math.round(y + h) + .5, Math.round(x) + .5, Math.round(y) + .5],
	        rowHeight = h / hv,
	        columnWidth = w / wv;
	    for (var i = 1; i < hv; i++) 
	    {
	        path = path.concat(["M", Math.round(x) + .5, Math.round(y + i * rowHeight) + .5, "H", Math.round(x + w) + .5]);
	    }
	    for (i = 1; i < wv; i++) 
	    {
	        path = path.concat(["M", Math.round(x + i * columnWidth) + .5, Math.round(y) + .5, "V", Math.round(y + h) + .5]);
	    }
	    return this.paper.path(path.join(",")).attr({stroke: color});
	},
	
	drawTitle: function()
	{
		if (this.options.title == '') return;
		this.title = this.paper.text(this.options.chartWidth / 2 + this.options.chartLeft, this.options.chartTop - 30, this.options.title);
    	this.title.attr({stroke: 'none', fill: this.palette.strokeColor, "font-size": this.options.titleSize});
	}
	
});
		
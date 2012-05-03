var HistogramSeries = new Class(
{
	Implements: [Options],
	type: 'block',
	title: '',
	values: [],
	max: 0,
	min: 0,
	options:
	{
		shadow: false,
		emboss: true,
		styles: {}
	},

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
		var renderer = Class.Empty;
		
		switch(this.type)
		{
		case 'block':
			
			renderer = new BlockSeriesRenderer(chart, this, index);
		}
		
		if (!renderer) return;
		
		renderer.draw();
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
		var fillSwatch = this.chart.palette.swatches[this.index];
		
		this.series.values.each(function(val, i)
		{
			var columnWidth = (this.chart.columnWidth / this.chart.blockSeriesCount) * (1 - this.chart.options.columnMargin);

			var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth) + (this.chart.blockSeriesDrawn * columnWidth);
			var columnLeft = this.chart.columnWidth * i + columnOffset;
			
			var x = this.chart.options.chartLeft + columnLeft;
			var columnHeight = this.chart.options.chartHeight * val / this.chart.max;
			var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight;
			
			var column = this.chart.paper.rect(x, y, columnWidth, columnHeight);
			column.attr({fill: fillSwatch});
			
			if (this.series.options.emboss)
			{
				column.emboss();
//				highlights = this.chart.paper.rect(x, y, columnWidth, columnHeight);
//				highlights.attr({fill: fillSwatch});
//				highlights.emboss();
			}

			if (this.series.options.shadow)
			{
				column.dropShadow(5, 1, 1, 0.1);
			}
			
			
		}.bind(this));

		this.chart.blockSeriesDrawn++;
	}
});

var Histogram = new Class(
{
	Extends: Chart,
	Implements: [Options],
	series: [],
	labels: [],
	max: 0,
	
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
		columnMargin: 0.2
	},
	
	initialize: function(id, options, labels)
	{
		this.parent(id)
		this.setOptions(options);
		this.labels = labels;
	},
	
	addSeries: function(series)
	{
		this.series.push(series);
	},
	
	draw: function()
	{
		this.createChart();

		if (this.labels.length == 0)
		{
			count = this.series[0].length;
			for(i = 1; i <= count; ++i)
			{
				this.labels.push(i);
			}
		}
		
		this.max = this.options.max;
		this.series.each(function(s)
		{
			if (s.max > this.max) this.max = s.max;
		}.bind(this));
		
		this.max = Math.ceil(this.max / this.options.ticks) * this.options.ticks;
		
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

		var idx = 0;
		this.series.each(function(s)
		{
			s.draw(this, idx++);
		}.bind(this));
		
		this.drawLabels();
		this.drawTicks();
		this.drawLegend();
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
		var increment = this.max / this.options.ticks;
		
		var tick = 0;
		var y = this.options.chartTop + this.options.chartHeight;
		var ystep = this.options.chartHeight / this.options.ticks;
		
		for(tick = 0; tick <= this.max; tick += increment)
		{
			var text = this.paper.text(this.options.chartLeft - 10, y, tick);
			text.attr({stroke: 'none', fill: this.palette.strokeColor, "font-size": this.options.labelSize, "text-anchor": "end"});
			y -= ystep;
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
	}
	
});
		
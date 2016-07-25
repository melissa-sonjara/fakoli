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
		emboss: false,
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
		areaFillOpacity: 0,
		showValues: false,
		color: 0,
		applyOffset: true,
		joinPrevious: false
	},
	columns: [],
	renderer: Class.Empty,
	chart: Class.Empty,
	group: Class.Empty,
	
	initialize: function(type, title, values, options)
	{
		this.type = type;
		this.title = title;
		this.values = values;
		this.max = this.values.max();
		this.min = this.values.min();
		
		this.setOptions(options);
	},
	
	getRenderer: function(chart, index)
	{
		if (!this.renderer)
		{
			this.renderer = chart.getSeriesRenderer(this, index);
		}
		
		return this.renderer;
	},
	
	draw: function(chart, index)
	{
		var renderer = this.getRenderer(chart, index);
		if (renderer) renderer.draw();
	},
	
	drawFill: function(chart, index)
	{
		var renderer = this.getRenderer(chart, index);
		if (renderer) renderer.drawFill();
	},
	
	drawDots: function(chart, index)
	{
		var renderer = this.getRenderer(chart, index);
		if (renderer) renderer.drawDots();
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

var VerticalBlockSeriesRenderer = new Class(
{
	chart: Class.Empty,
	series: Class.Empty,
	index: 0,
	dropShadow: null,
	
	initialize: function(chart, series, index)
	{
		this.chart = chart;
		this.series = series;
		this.index = index;
	},
	
	addShadow: function(shape)
	{
		if (!this.dropShadow)
		{
			this.dropShadow = this.chart.paper.filter(Snap.filter.shadow(1, 1, 5, 0.1));
		}
		shape.attr({filter: this.dropShadow});
	},
	
	getColor: function(i)
	{
		if (this.series.options.colorMode == 'fixed') return this.chart.palette.getColor(this.series.options.color);
		return this.chart.palette.getColor((this.series.options.colorMode == 'series') ? this.index : i);
	},
	
	draw: function()
	{
		this.series.values.each(function(val, i)
		{
			var fillSwatch = this.getColor(i);			

			var columnWidth = this.chart.blockWidth;

			var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth);
			if (this.series.options.applyOffset)
			{
				columnOffset += this.chart.blockSeriesDrawn * columnWidth;
			}
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
				this.addShadow(column);
			}
			
			if (this.series.options.showValues)
			{
				if (!val) val = 0;
				
				this.chart.paper.text(x + columnWidth / 2, y - 8, val + this.chart.options.units);
			}
			
			column.mouseover(function(e) { this.series.fireEvent('mouseOver', [e, i]); this.series.showToolTip(e, i);}.bind(this));
			column.mouseout(function(e) { this.series.fireEvent('mouseOut', [e, i]);  this.series.hideToolTip();}.bind(this));
			column.click(function() { this.series.fireEvent('click', i); }.bind(this));
			
			this.series.columns.push(column);
			
		}.bind(this));

		this.chart.blockSeriesDrawn++;
	},
	
	drawDots: function()
	{	
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

			this.series.columns[i].animate({'y' :y, 'height': columnHeight}, 1000, mina.easeinout);
		}.bind(this));
	}
});


var HorizontalBlockSeriesRenderer = new Class(
{
	chart: Class.Empty,
	series: Class.Empty,
	index: 0,
	dropShadow: null,
	
	initialize: function(chart, series, index)
	{
		this.chart = chart;
		this.series = series;
		this.index = index;
	},

	addShadow: function(shape)
	{
		if (!this.dropShadow)
		{
			this.dropShadow = this.chart.paper.filter(Snap.filter.shadow(1, 1, 5, 0.1));
		}
		shape.attr({filter: this.dropShadow});
	},
	
	getColor: function(i)
	{
		if (this.series.options.colorMode == 'fixed') return this.chart.palette.getColor(this.series.options.color);
		return this.chart.palette.getColor((this.series.options.colorMode == 'series') ? this.index : i);
	},
	
	draw: function()
	{
		this.series.values.each(function(val, i)
		{
			var fillSwatch = this.getColor(i);			

			var columnWidth = this.chart.blockWidth;

			var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth);
			if (this.series.options.applyOffset)
			{
				columnOffset += this.chart.blockSeriesDrawn * columnWidth;
			}
			
			var columnTop = this.chart.columnWidth * i + columnOffset;
			
			var x = this.chart.options.chartLeft;
			var columnHeight = this.chart.options.chartWidth * val / this.chart.range();
			var y = this.chart.options.chartTop + columnTop;
			
			this.chart.options.chartHeight - columnHeight - this.chart.xAxisOffset;
			
			var column = this.chart.paper.rect(x, y, columnHeight, columnWidth);
			column.attr({fill: fillSwatch});
			
			if (this.series.options.emboss)
			{
				column.emboss();
			}

			if (this.series.options.shadow)
			{
				this.addShadow(column);
			}
			
			if (this.series.options.showValues)
			{
				if (!val) val = 0;
				
				this.chart.paper.text(x + columnHeight + 5, y + columnWidth / 2, val + this.chart.options.units).attr({'text-anchor': 'start'});
			}
			
			column.mouseover(function(e) { this.series.fireEvent('mouseOver', [e, i]);  this.series.showToolTip(e, i); }.bind(this));
			column.mouseout(function(e) { this.series.fireEvent('mouseOut', [e, i]);  this.series.hideToolTip();}.bind(this));
			column.click(function() { this.series.fireEvent('click', i); }.bind(this));
			
			this.series.columns.push(column);
			
		}.bind(this));

		this.chart.blockSeriesDrawn++;
	},
	
	drawDots: function()
	{
		
	},
	
	drawFill: function()
	{
	},
	
	// Morph this series to match the values of the supplied series
	morph: function(series)
	{
		series.values.each(function(val, i)
		{
			var columnHeight = this.chart.options.chartWidth * val / this.chart.range();

			this.series.columns[i].animate({'width': columnHeight}, 1000, mina.easeinout);
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
	
	getColor: function(i)
	{
		if (this.series.options.colorMode == 'fixed') return this.chart.palette.getColor(this.series.options.color);
		return this.chart.palette.getColor((this.series.options.colorMode == 'series') ? this.index : i);
	},
	
	draw: function()
	{
		this.drawLine();
		if (this.series.group === null || this.series.group === undefined) 
		{
			this.drawDots();	
		}
	},
	
	drawLine: function()
	{
		var lineColor = this.getColor(this.index);
		var fillColor = this.chart.options.chartBackground;
		var p = this.calculatePath(this.series, false);

		
		this.path = this.chart.paper.path(p).attr({"stroke-width": this.series.options.strokeWidth, stroke: lineColor, fill: 'none'});
		
	},
	
	drawDots: function()
	{
		var lineColor = this.getColor(this.index);
		var fillColor = this.chart.options.chartBackground;
		this.coords.each(function(c, i) {
			if (c !== null)
			{
				var dot = this.chart.paper.circle(c.x, c.y, this.series.options.symbolSize).attr({"stroke-width": this.series.options.strokeWidth, stroke: lineColor, fill: (this.series.options.indicateTooltips && this.series.hasTooltip(i) ) ? lineColor : fillColor, 'cursor': 'pointer'});
				dot.mouseover(function(e) {dot.animate({'r': this.series.options.symbolSize * 2}, 250, mina.easein); this.series.fireEvent('mouseOver', [e, i]); this.series.showToolTip(e, i);}.bind(this));
				dot.mouseout(function(e) {dot.animate({'r': this.series.options.symbolSize}, 250, mina.easeout);  this.series.fireEvent('mouseOut', [e, i]); this.series.hideToolTip();}.bind(this));
				dot.click(function() { this.series.fireEvent('click', i); }.bind(this));
				this.dots.push(dot);
			}
			else
			{
				this.dots.push(null);
			}
		}.bind(this));	
	},
	
	drawFill: function()
	{
		var lineColor = this.getColor(this.index);
		
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
		var startX = -999;
		var lastX = -999;
		
		var values = Array.clone(series.values);
		if (series.group !== null && series.options.joinPrevious)
		{
			for(var i = 0; i < values.length; ++i)
			{
				if (values[i] !== null)
				{
					if (i > 0)
					{
						values[i-1] = series.group.getValue(i-1);
					}
					break;
				}
			}
		}
		
		var first = true;
		
		values.each(function(val, i)
		{
			if (val !== null)
			{
				var columnOffset = this.chart.getColumnOffset();
				var columnCenter = this.chart.columnWidth * i + columnOffset;
				
				
				var x = this.chart.options.chartLeft + columnCenter;
				var columnHeight = this.chart.options.chartHeight * val / this.chart.range() + this.chart.xAxisOffset;
				var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight;
				
				if (!first || !this.series.options.joinPrevious)
				{
					this.coords.push({'x': x, 'y': y});
				}
				else
				{
					this.coords.push(null);
				}
				
				first = false;
				p += cmd + x + "," + y;
				cmd = "L";			
				
				if (startX == -999) startX = x;
				lastX = x;			
			}
			else
			{
				this.coords.push(null);
				cmd = "M";
			}		
			
			if (i == series.values.length - 1 && closed)
			{
				y = this.chart.options.chartTop + this.chart.options.chartHeight - this.chart.xAxisOffset;
				p += "L" + lastX + "," + y + "L" + startX + "," + y + "Z";
			}
			
		}.bind(this));
		
		return p;
	},
	
	// Morph this series to match the values of the supplied series
	morph: function(series)
	{
		var p = this.calculatePath(series);
		var lineColor =  this.getColor(series.index);
		
		var fillColor = this.chart.options.chartBackground;
		this.path.animate({'path': p, 'stroke': lineColor}, 1000, mina.easeinout);
		
		if (this.series.options.areaFill)
		{
			if (!this.fill) 
			{
				this.drawFill();
			}
			else
			{
				var f = this.calculatePath(series, true);
				this.fill.animate({'path': f, stroke: lineColor, fill: lineColor}, 1000, mina.easeinout);
			}
		}
		
		this.dots.each(function(dot, i) 
		{
			if (this.coords[i] !== null)
			{
				dot.animate({'cy': this.coords[i].y, 'stroke': lineColor, fill: (this.series.options.indicateTooltips && this.series.hasTooltip(i) ) ? lineColor : fillColor}, 1000, mina.easeinout);
			}
		}.bind(this));
	}		
});

var VerticalHistogramAxisRenderer = new Class(
{
	chart: Class.Empty,
	ticks: [],
	
	initialize: function(chart)
	{
		this.chart = chart;
	},
	
	calculateColumnWidth: function(count)
	{
		return this.chart.options.chartWidth / count;
	},
	
	drawLabels: function()
	{
		this.chart.labels.each(function(text, index)
		{
			var x = this.options.chartLeft + this.columnWidth * index + (this.getColumnOffset());
			var y = this.options.chartTop + this.options.chartHeight + 20 + (text.count("\n") * this.options.labelSize / 2);
			
			var label = this.paper.text(x, y, text);
			var i = "Tooltip";
			label.attr({stroke: 'none', fill: this.palette.strokeColor, "font-size": this.options.labelSize, "text-anchor": "middle"});
			label.mouseover(function(e) { this.fireEvent('mouseOver', [e, i]); this.showToolTip(e, i);}.bind(this));
			label.mouseout(function(e) { this.fireEvent('mouseOut', [e, i]);  this.hideToolTip();}.bind(this));
			label.click(function() { this.fireEvent('click', i); }.bind(this));
		}.bind(this.chart));
	},
	
	drawTicks: function()
	{
		var chart = this.chart;
		
		var increment = chart.range() / chart.options.ticks;
		
		if (increment == 0) return;
		
		var tick = chart.min;
		var idx = 0;
		var y = chart.options.chartTop + chart.options.chartHeight;
		var ystep = chart.options.chartHeight / chart.options.ticks;
		
		for(tick = chart.min; tick <= chart.max; tick += increment)
		{
			if (chart.yAxisLabels.length > 0)
			{
				label = chart.yAxisLabels[idx];
			}
			else
			{
				label = number_format(tick, 0) + chart.options.units;
			}
			var text = chart.paper.text(chart.options.chartLeft - 10, y, label);
			text.attr({stroke: 'none', fill: chart.palette.strokeColor, "font-size": chart.options.labelSize, "text-anchor": "end"});
			this.ticks.push(text);
			y -= ystep;
			++idx;
		}
	},
	
	drawGrid: function(x, y, w, h, wv, hv, color) 
	{
	    color = color || "#000";
	    var path = '',
	        rowHeight = h / hv,
	        columnWidth = w / wv,
	        chart = this.chart;
	    for (var i = 1; i < hv; i++) 
	    {
	        path = chart.path(path, ["M", (Math.round(x) + .5), (Math.round(y + i * rowHeight) + .5)], ["H", (Math.round(x + w) + .5)]);
	    }
	    for (i = 1; i < wv; i++) 
	    {
	        path = chart.path(path, ["M", (Math.round(x + i * columnWidth) + .5), (Math.round(y) + .5)], ["V", (Math.round(y + h) + .5)]);
	    }
	    return this.chart.paper.path({d: path, stroke: color});
	},
});

var HorizontalHistogramAxisRenderer = new Class(
{
	chart: Class.Empty,
	ticks: [],

	initialize: function(chart)
	{
		this.chart = chart;
	},
	
	calculateColumnWidth: function(count)
	{
		return this.chart.options.chartHeight / count;
	},
	
	showToolTip: function(evt, idx)
	{
		if (idx > this.chart.labelTooltips.length) return;
		
		var text = this.chart.labelTooltips[idx];
		if (text == '') return;
		
		showTextToolTip(this.chart.container, evt, this.chart.id + "_tooltip", text);		
	},
	
	hideToolTip: function()
	{
		hideToolTip(this.chart.id + "_tooltip");
	},	

	drawLabels: function()
	{
		this.chart.labels.each(function(text, index)
		{
			let chart = this.chart;
			var x = chart.options.chartLeft - 5;
			var y = chart.options.chartTop + chart.columnWidth * index + (chart.getColumnOffset());
			//+ this.options.chartHeight + 20 + (text.count("\n") * this.options.labelSize / 2);
			
			var label = chart.paper.text(x, y, text);
			label.attr({stroke: 'none', fill: chart.palette.strokeColor, "font-size": chart.options.labelSize, "text-anchor": "end"});
			label.mouseover(function(e) { chart.fireEvent('mouseOver', [e, index]); this.showToolTip(e, index);}.bind(this));
			label.mouseout(function(e) { chart.fireEvent('mouseOut', [e, index]);  this.hideToolTip();}.bind(this));
			label.click(function() { chart.fireEvent('click', index); }.bind(this));
		}.bind(this));
	},
	
	drawTicks: function()
	{
		var chart = this.chart;
		
		var increment = chart.range() / chart.options.ticks;
		
		if (increment == 0) return;
		
		var tick = chart.min;
		var idx = 0;
		var x = chart.options.chartLeft;
		var xstep = chart.options.chartWidth / chart.options.ticks;
		
		for(tick = chart.min; tick <= chart.max; tick += increment)
		{
			if (chart.yAxisLabels.length > 0)
			{
				label = chart.yAxisLabels[idx];
			}
			else
			{
				label = number_format(tick, 0) + chart.options.units;
			}
			var text = chart.paper.text(x, chart.options.chartTop - 10, label);
			text.attr({stroke: 'none', fill: chart.palette.strokeColor, "font-size": chart.options.labelSize, "text-anchor": "middle"});
			this.ticks.push(text);
			x += xstep;
			++idx;
		}
	},
	
	drawGrid: function(x, y, w, h, hv, wv, color) 
	{
	    color = color || "#000";
	    var path = '',
	        rowHeight = h / hv,
	        columnWidth = w / wv,
	        chart = this.chart;
	    for (var i = 1; i < hv; i++) 
	    {
	        path = chart.path(path, ["M", Math.round(x) + .5, Math.round(y + i * rowHeight) + .5], ["H", Math.round(x + w) + .5]);
	    }
	    for (i = 1; i < wv; i++) 
	    {
	        path = chart.path(path, ["M", Math.round(x + i * columnWidth) + .5, Math.round(y) + .5], ["V", Math.round(y + h) + .5]);
	    }
	    return this.chart.paper.path({d: path, stroke: color});
	},

});

var Histogram = new Class(
{
	Extends: Chart,
	Implements: [Options],
	series: [],
	labels: [],
	labelTooltips: [],
	yAxisLabels: [],
	max: 0,
	min: 0,
	xAxisOffset: 0,
	linearOnly: true,
	dropShadow: null,
	
	options:
	{
		palette: 'standard',
		width: 600,
		height: 400,
		orientation: 'vertical',
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
		this.createAxisRenderer();	
	},

	getSeriesRenderer: function(series, index)
	{
		var renderer = null;
		
		switch(series.type)
		{
		case 'block':
			
			renderer = (this.options.orientation == 'horizontal') ? 
					new HorizontalBlockSeriesRenderer(this, series, index) : 
					new VerticalBlockSeriesRenderer(this, series, index);
			break;
			
		case 'line':
		default:
			
			renderer = new LineSeriesRenderer(this, series, index);
			break;
		}
		
		return renderer;
	},
	
	addShadow: function(shape)
	{
		if (!this.dropShadow)
		{
			this.dropShadow = this.paper.filter(Snap.filter.shadow(1, 1, 7, 0.2));
		}
		shape.attr({filter: this.dropShadow});
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
	
	createAxisRenderer: function()
	{
		switch(this.options.orientation)
		{
		case 'horizontal':
			this.axisRenderer = new HorizontalHistogramAxisRenderer(this);
			break;
			
		case 'vertical':
		default:
		
			this.axisRenderer = new VerticalHistogramAxisRenderer(this);
		}
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
			for(var idx = 1; idx <= count; ++idx)
			{
				this.labels.push(idx);
			}
		}
		
		this.max = this.options.max;
		this.min = this.options.min;
		
		this.series.each(function(s)
		{
			if (s.max > this.max) this.max = s.max;
			if (s.min < this.min) this.min = s.min;
			if (s.type != 'line') this.linearOnly = false;
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
			this.addShadow(grid);
		}
		
		var count = this.labels.length;
		if (this.linearOnly) count -= 1;
		
		this.columnWidth = this.axisRenderer.calculateColumnWidth(count);
		
		this.axisRenderer.drawGrid(this.options.chartLeft, this.options.chartTop, this.options.chartWidth, this.options.chartHeight, count, this.options.ticks, this.palette.strokeColor);
	},
	
	getColumnOffset: function()
	{
		return this.linearOnly ? 0 : this.columnWidth / 2;
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
		this.axisRenderer.drawLabels();
	},
	
	drawTicks: function()
	{
		this.axisRenderer.drawTicks();
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
	    return this.paper.path(path.join(" ")).attr({stroke: color});
	},
	
	drawTitle: function()
	{
		if (this.options.title == '') return;
		this.title = this.paper.text(this.options.chartWidth / 2 + this.options.chartLeft, this.options.chartTop - 30, this.options.title);
    	this.title.attr({stroke: 'none', fill: this.palette.strokeColor, "font-size": this.options.titleSize});
	},
	
	
	drawLegend: function()
	{
		if (this.options.legend)
		{
			var x = this.options.legendX;
			var y = this.options.legendY;
			var s = this.options.legendSwatchSize || 20;
			var h = this.options.legendLineHeight || 30;
			
			this.series.each(function(series, index)
			{
				var title = series.title;
				
				var rect = this.paper.rect(x, y, s, s, 3);
				rect.attr({fill:this.palette.swatches[index], stroke: this.palette.strokeColor, "stroke-width": this.options.strokeWidth});
				
				var text = this.paper.text(x + h, y + h / 2, title);
				
				text.attr({"text-anchor": "start", fill: this.palette.strokeColor, stroke: "none" , opacity: 1, "font-size": this.options.labelSize});
					
				rect.mouseover(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				rect.mousemove(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				rect.mouseout(function(e) { this.fireEvent('legendOut', [e, index]); }.bind(this));
				
				text.mouseover(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				text.mousemove(function(e) { this.fireEvent('legendOver', [e, index]); }.bind(this));
				text.mouseout(function(e) { this.fireEvent('legendOut', [e, index]); }.bind(this));

				y+= h;
			}.bind(this));
		}
	}
	
});

var GroupedHistogramSeries = new Class({
	Implements: [Options, Events],
	type: 'block',
	title: '',
	children: [],
	max: 0,
	min: 0,
	

	initialize: function(type, title, options)
	{
		this.type = type;
		this.title = title;
		
		this.setOptions(options);
	},
	
	addSeries: function(series)
	{
		series.group = this;
		this.children.push(series);
		this.children.each(function(child)
		{
			if (child.max > this.max) this.max = child.max;
			if (child.min < this.min) this.min = child.min;
		}.bind(this));
		
		return this;
	},
	
	getValue: function(idx)
	{
		var value = null;
		this.children.each(function(child)
		{
			if (value !== null) return;
			value = child.values[idx];
		});
		
		return value;
	},
	
	draw: function(chart, index)
	{
		this.children.each(function(child)
		{
			child.draw(chart, index);
		});
		
		this.drawDots(chart, index);
	},
	
	drawFill: function(chart, index)
	{
		this.children.each(function(child)
		{
			child.drawFill(chart, index);
		});
	},
	
	drawDots: function(chart, index)
	{
		this.children.each(function(child)
		{
			child.drawDots(chart, index);
		});
	},
	
	morph: function(series)
	{
		this.children.each(function(child, idx)
		{
			child.morph(series.children[idx]);
		});
	},
	
	hasTooltip: function(idx)
	{
		var found = false;
		
		this.children.each(function(child)
		{
			if (child.hasTooltip(idx)) found = true;
		});
		
		return found;
	},
	
	showToolTip: function(evt, idx)
	{
		this.children.each(function(child)
		{
			child.showTooltip(evt, idx);
		});
		if (idx > this.options.toolTips.length) return;
		},
	
	hideToolTip: function()
	{
		hideToolTip(this.chart.id + "_tooltip");
	}
});
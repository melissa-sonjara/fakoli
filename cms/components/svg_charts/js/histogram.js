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
		strokeWidth: 1,
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
	
	connectToChart: function(chart, index)
	{
		this.chart = chart;
		this.index = index;
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
	},
	
	collectLegends: function(chart, index, legends)
	{
		var renderer = this.getRenderer(chart, index);
		if (renderer) renderer.collectLegends(legends);
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
			column.attr({fill: fillSwatch, 'stroke-width': this.series.options.strokeWidth, 'stroke': this.chart.palette.strokeColor});
			
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
				
				this.chart.paper.text(x + columnWidth / 2, y - 8, val + this.chart.options.units)
					.attr({"text-anchor": "middle", 'font-size': this.chart.options.labelSize});
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
	},
	
	collectLegends: function(legends)
	{
		legends.push({title: this.series.title, color: this.getColor(this.index)});
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
			column.attr({fill: fillSwatch, 'stroke-width': this.series.options.strokeWidth, 'stroke': this.chart.palette.strokeColor});
			
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
	},
	
	collectLegends: function(legends)
	{
		legends.push({title: this.series.title, color: this.getColor(this.index)});
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
	},
	
	collectLegends: function(legends)
	{
		legends.push({title: this.series.title, color: this.getColor(this.index)});
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
			var chart = this.chart;
			var x = chart.options.chartLeft + chart.columnWidth * index + (chart.getColumnOffset());
			var y = chart.options.chartTop + chart.options.chartHeight + chart.options.labelOffset;
			
			var decorations = chart.labelDecorations[index];
			var attrs = {stroke: 'none', fill: chart.palette.strokeColor, "font-size": chart.options.labelSize, "text-anchor": chart.options.labelAnchor};
			Object.append(attrs, decorations);

			if (chart.options.labelAngle)
			{
				attrs.transform = "rotate(" + chart.options.labelAngle + "," + x + "," + y + ")";
				var label = chart.paper.text(x, y, text);
				label.attr(attrs);
			}
			else
			{
				var label = chart.paper.multitext(x, y, text, chart.columnWidth, attrs);
			}
			
			var i = "Tooltip";

			label.mouseover(function(e) { chart.fireEvent('mouseOver', [e, i]); this.showToolTip(e, index);}.bind(this));
			label.mouseout(function(e) { chart.fireEvent('mouseOut', [e, i]);  this.hideToolTip();}.bind(this));
			label.click(function() { chart.fireEvent('click', i); }.bind(this));
		}.bind(this));
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
	    return this.chart.paper.path({d: path, stroke: color, 'class': 'grid'});
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
		//if (idx > this.chart.labelTooltips.length) return;
		
		var text = this.chart.labelTooltips[idx];
		if (text == '') text = "N/A";
		
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
			var chart = this.chart;
			var decorations = this.chart.labelDecorations[index];
			var x = chart.options.chartLeft - 5;
			var y = chart.options.chartTop + chart.columnWidth * index + (chart.getColumnOffset());
			//+ this.options.chartHeight + 20 + (text.count("\n") * this.options.labelSize / 2);
			
			var label = chart.paper.text(x, y, text);
			
			var attrs = {stroke: 'none', fill: chart.palette.strokeColor, "font-size": chart.options.labelSize, "text-anchor": "end"};
			Object.append(attrs, decorations);
			
			label.attr(attrs);
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
	    return this.chart.paper.path({d: path, stroke: color, 'class': 'grid'});
	},

});

var Histogram = new Class(
{
	Extends: Chart,
	Implements: [Options],
	series: [],
	labels: [],
	labelTooltips: [],
	labelDecorations: [],
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
		labelAngle: 0,
		labelAnchor: 'middle',
		labelOffset: 20,
		strokeWidth: 2,
		animate: true,
		shadow: false,
		gridStrokeWidth: 1,
		ticks: 10,
		units: "",
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
		series.connectToChart(this, this.series.length - 1);
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
		var chart = this.chart;
		
	    color = color || "#000";
	    var path = chart.path(["M", Math.round(x) + .5, Math.round(y) + .5], 
	    					  ["L", Math.round(x + w) + .5, Math.round(y) + .5, 
	    					   Math.round(x + w) + .5, Math.round(y + h) + .5, 
	    					   Math.round(x) + .5, Math.round(y + h) + .5, 
	    					   Math.round(x) + .5, Math.round(y) + .5]);
	    var rowHeight = h / hv,
	        columnWidth = w / wv;
	    for (var i = 1; i < hv; i++) 
	    {
	        path = chart.path(path, ["M", Math.round(x) + .5, Math.round(y + i * rowHeight) + .5], ["H", Math.round(x + w) + .5]);
	    }
	    for (i = 1; i < wv; i++) 
	    {
	        path = chart.path(path, ["M", Math.round(x + i * columnWidth) + .5, Math.round(y) + .5], ["V", Math.round(y + h) + .5]);
	    }
	    return this.paper.path({d: path, stroke: color, 'class': 'grid'});
	},
	
	drawTitle: function()
	{
		if (this.options.title == '') return;
		this.title = this.paper.text(this.options.chartWidth / 2 + this.options.chartLeft, this.options.chartTop - 30, this.options.title);
    	this.title.attr({stroke: 'none', fill: this.palette.strokeColor, "font-size": this.options.titleSize, 'text-anchor': 'middle', 'class': 'title'});
	},
	
	
	drawLegend: function()
	{
		if (this.options.legend)
		{
			var x = this.options.legendX;
			var y = this.options.legendY;
			var s = this.options.legendSwatchSize || 20;
			var h = this.options.legendLineHeight || 30;
			
			var legends = [];
			this.series.each(function(series, index)
			{
				series.collectLegends(this, index, legends);
			}.bind(this));
			
			legends.each(function(legend, index)
			{
				var rect = this.paper.rect(x, y, s, s, 3);
				rect.attr({fill: legend.color, stroke: this.palette.strokeColor, "stroke-width": this.options.strokeWidth});
				
				var text = this.paper.text(x + h, y + h / 2, legend.title);
				
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
	Extends: HistogramSeries,
	Implements: [Options, Events],
	type: 'block',
	title: '',
	children: [],	
	min: Infinity,
	max: -Infinity,
	
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

	connectToChart: function(chart, index)
	{
		this.chart = chart;
		this.index = index;
		this.children.each(function(child, i)
		{
			child.connectToChart(chart, index + i);
		});
	},

	getRange: function()
	{
		var max = -Infiinity;
		var min = Infinity;
	
		var hasRange = false;
		
		this.children.each(function(child)
		{
			if (child.values.length == 0) return;
			
			child.values.each(function (value)
			{
				if (value == null) return;
				
				if (value > max) max = value;
				if (value < min) min = value;
				
				hasRange = true;
			});
		});
		
		return hasRange ? {'max': max, 'min': min} : false;
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
	},
	
	collectLegends: function(chart, index, legends)
	{
		this.children.each(function(child, idx)
		{
			child.collectLegends(chart, idx, legends);
		});
	}
});

var StackedHistogramSeries = new Class({

	Extends: HistogramSeries,
	Implements: [Options, Events],
	type: 'block',
	title: '',
	children: [],
	max: 0,
	min: 0,
	totals: [],
	
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
		
		this.calculateTotals();
		this.min = this.totals.min();
		this.max = this.totals.max();
		
		return this;
	},

	connectToChart: function(chart, index)
	{
		this.chart = chart;
		this.index = index;
		this.children.each(function(child, i)
		{
			child.connectToChart(chart, index + i);
		});
	},
	
	calculateTotals: function()
	{
		this.totals = [];
		
		this.children[0].values.each(function() { this.totals.push(0); }.bind(this));
		
		this.children.each(function(child)
		{
			child.values.each(function (value, idx)
			{
				this.totals[idx] += value;
			}.bind(this));
		}.bind(this));
	},
	
	getValue: function(idx)
	{
		// Return total value
		return this.totals[idx];
	},
	
	
	getRenderer: function(chart, index)
	{
		if (!this.renderer)
		{
			switch(this.type)
			{
			case 'block':
				
				this.renderer = (chart.options.orientation == 'horizontal') ? 
						new StackedHorizontalBlockSeriesRenderer(chart, this, index) : 
						new StackedVerticalBlockSeriesRenderer(chart, this, index);
				break;
				
			case 'line':
				
				this.renderer = new StackedLineSeriesRenderer(chart, this, index);
				break;
			}
		}
		
		return this.renderer;
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
	},
	
	collectLegends: function(chart, index, legends)
	{
		this.children.each(function(child, idx)
		{
			child.collectLegends(chart, idx, legends);
		});
	}	
});






var StackedVerticalBlockSeriesRenderer = new Class(
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
		if (this.series.children[i].options.colorMode == 'fixed') return this.chart.palette.getColor(this.series.children[i].options.color);
		return this.chart.palette.getColor(this.index + i);
	},
	
	draw: function()
	{
		var runningTotals = [];
		this.series.children[0].values.each(function() { runningTotals.push(0);});
		
		this.series.children.each(function(child, idx)
		{
			child.values.each(function(val, i)
			{
				var runningTotal = runningTotals[i];
				if (!runningTotal) runningTotal = 0;
				
				var fillSwatch = this.getColor(idx);			
	
				var columnWidth = this.chart.blockWidth;
	
				var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth);
				if (this.series.options.applyOffset)
				{
					columnOffset += this.chart.blockSeriesDrawn * columnWidth;
				}
				var columnLeft = this.chart.columnWidth * i + columnOffset;
				
				var x = this.chart.options.chartLeft + columnLeft;
				var columnHeight = this.chart.options.chartHeight * val / this.chart.range();
				var columnBottom = this.chart.options.chartHeight * runningTotal / this.chart.range();
				
				var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight - columnBottom - this.chart.xAxisOffset;
				
				var column = this.chart.paper.rect(x, y, columnWidth, columnHeight);
				column.attr({fill: fillSwatch, 'stroke-width': this.series.options.strokeWidth, 'stroke': this.chart.palette.strokeColor});
				
				runningTotals[i] = val + runningTotal;
				
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
				
				column.mouseover(function(e) { child.fireEvent('mouseOver', [e, i]); child.showToolTip(e, i);}.bind(this));
				column.mouseout(function(e) { child.fireEvent('mouseOut', [e, i]);  child.hideToolTip();}.bind(this));
				column.click(function() { child.fireEvent('click', i); }.bind(this));
				
				child.columns.push(column);
				
			}.bind(this));
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
		/*series.values.each(function(val, i)
		{
			var columnHeight = this.chart.options.chartHeight * val / this.chart.range() + this.chart.xAxisOffset;
			var y = this.chart.options.chartTop + this.chart.options.chartHeight - columnHeight;

			this.series.columns[i].animate({'y' :y, 'height': columnHeight}, 1000, mina.easeinout);
		}.bind(this));*/
	}
});


var StackedHorizontalBlockSeriesRenderer = new Class(
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
		if (this.series.children[i].options.colorMode == 'fixed') return this.chart.palette.getColor(this.series.children[i].options.color);
		return this.chart.palette.getColor(this.index + i);
	},
	
	draw: function()
	{
		var runningTotals = [];
		this.series.children[0].values.each(function() { runningTotals.push(0);});
		
		this.series.children.each(function(child, idx)
		{
			child.values.each(function(val, i)
			{
				var runningTotal = runningTotals[i];
				if (!runningTotal) runningTotal = 0;
				var fillSwatch = this.getColor(idx);			
	
				var columnWidth = this.chart.blockWidth;
	
				var columnOffset = (this.chart.options.columnMargin / 2 * this.chart.columnWidth);
				if (this.series.options.applyOffset)
				{
					columnOffset += this.chart.blockSeriesDrawn * columnWidth;
				}
				
				var columnTop = this.chart.columnWidth * i + columnOffset;
				
				var columnLeft = this.chart.options.chartWidth * runningTotal / this.chart.range();
				
				var x = this.chart.options.chartLeft + columnLeft;
				var columnHeight = this.chart.options.chartWidth * val / this.chart.range();
				var y = this.chart.options.chartTop + columnTop;
								
				var column = this.chart.paper.rect(x, y, columnHeight, columnWidth);
				column.attr({fill: fillSwatch, 'stroke-width': this.series.options.strokeWidth, 'stroke': this.chart.palette.strokeColor});
				
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
					
					this.chart.paper.text(x + columnHeight + 5, y + columnWidth / 2, val + this.chart.options.units)
						.attr({'text-anchor': 'start', 'font-size': this.chart.options.labelSize});
				}
				
				column.mouseover(function(e) { child.fireEvent('mouseOver', [e, i]);  child.showToolTip(e, i); }.bind(this));
				column.mouseout(function(e) { child.fireEvent('mouseOut', [e, i]);  child.hideToolTip();}.bind(this));
				column.click(function() { child.fireEvent('click', i); }.bind(this));
				
				runningTotals[i] = val + runningTotal;
				
				child.columns.push(column);
				
			}.bind(this));
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
		/*series.values.each(function(val, i)
		{
			var columnHeight = this.chart.options.chartWidth * val / this.chart.range();

			this.series.columns[i].animate({'width': columnHeight}, 1000, mina.easeinout);
		}.bind(this));*/
	}
});
/**
 * Radar Plot
 */
 
var StraightRadarSeriesRenderer = new Class({

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
	
	calculatePath: function(series)
	{
		var chart = this.chart;
		var cmd = 'M';

		var path = [];
		this.coords = [];
		
		series.values.each(function(val, i)
		{
			var angle = chart.arcStep * i;
			
			var magnitude = chart.options.radius * val / chart.max;
		
			var point = chart.calculatePoint(angle, magnitude);
			path = chart.path(path, [cmd, point.x, point.y]);	
			cmd = 'L';
			
			this.coords.push(point);
		}.bind(this));
		
		path = chart.path(path, ['z']);
		return path;
	},
	
	draw: function()
	{
		var chart = this.chart;
		
		var path = this.calculatePath(this.series);
		
		var lineColor = this.getColor(this.index);
		var f = this.series.options.areaFill ? lineColor : 'none';
		
		this.path = chart.paper.path(path).attr({"stroke-width": this.series.options.strokeWidth, stroke: lineColor, 
												 fill: 'none'});
	},
	
	drawFill: function()
	{
		if (!this.series.options.areaFill) return;
		var chart = this.chart;
		
		var path = this.calculatePath(this.series);
		
		var lineColor = this.getColor(this.index);
		
		this.fill = chart.paper.path(path).attr({"stroke-width": this.series.options.strokeWidth, stroke: lineColor, 
												 fill: lineColor, 'fill-opacity': this.series.options.areaFillOpacity});
	},
	
	drawDots: function()
	{
		this.dots = [];
		
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

var SmoothedRadarSeriesRenderer = new Class({

	Extends: StraightRadarSeriesRenderer,
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
	
	calculatePath: function(series)
	{
		var chart = this.chart;
		var cmd = 'M';
		var tension = 0.25;
		
		var path = [];
		this.coords = [];
		
		var magnitude = chart.options.radius * series.values[0] / chart.max;
		var point = chart.calculatePoint(0, magnitude);
		
		path = chart.path(path, ['M', point.x, point.y]);

		this.coords.push(point);
		
		for(var i = 1; i < series.values.length + 1; ++i)
		{
			var idx = i % series.values.length;
			
			var angle = chart.arcStep * i;
			var s1angle = (chart.arcStep * (i - 1 + tension));
			var s2angle = (chart.arcStep * (i - tension));
		
			var prevmag = chart.options.radius * series.values[i - 1] / chart.max;
			var magnitude = chart.options.radius * series.values[idx] / chart.max;
			
			var s1 = chart.calculatePoint(s1angle, prevmag);
			var s2 = chart.calculatePoint(s2angle, magnitude);
			
			var point = chart.calculatePoint(angle, magnitude);
			
			path = chart.path(path, ['C', s1.x, s1.y], ['', s2.x, s2.y], ['', point.x, point.y]);
			
			if (idx) this.coords.push(point);
		}
		
		path = chart.path(path, ['z']);
		return path;
	}		
});


var RadarChart = new Class(
{	
	Extends: Chart,
	Implements: [Options],
	series: [],
	labels: [],
	max: 0,
	dropShadow: null,
	arcStep: 0,
	
	options:
	{
		palette: 'standard',
		width: 600,
		height: 600,
		cx: 300,
		cy: 300,
		radius: 280,
		chartBackground: "#fff",
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

	getSeriesRenderer: function(series, index)
	{
		var renderer = null;
		
		switch(series.type)
		{
		case 'smoothed':
			
			renderer = new SmoothedRadarSeriesRenderer(this, series, index);
			break;

		case 'straight':
		default:
		
			renderer = new StraightRadarSeriesRenderer(this, series, index);
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

		this.setupRadarChart();
		
		this.drawRadarChart();
	},
	
	calculatePoint: function(angle, magnitude)
	{
		return {x: this.options.cx + (Math.sin(angle) * magnitude),
				y: this.options.cy - (Math.cos(angle) * magnitude)};
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
	
	drawRadarChart: function()
	{
		this.drawTicks();
		this.drawRadarPlots();
		this.drawLabels();
		this.drawLegend();
		this.drawTitle();
	},
	
	setupRadarChart: function()
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
		}.bind(this));

		this.arcStep = 2 * Math.PI / this.labels.length;

		//this.axisRenderer.drawGrid(this.options.chartLeft, this.options.chartTop, this.options.chartWidth, this.options.chartHeight, count, this.options.ticks, this.palette.strokeColor);
	},
	
	drawTicks: function()
	{
		for(i = 0; i < this.options.ticks; ++i)
		{
			this.paper.circle(this.options.cx, this.options.cy, 
							  this.options.radius * (i + 1) / this.options.ticks)
							  .attr({'fill': 'none', 'stroke-width': 1, stroke: '#666', 'stroke-dasharray': '2,2'});
		}
		
		for(i = 0; i < this.labels.length; ++i)
		{
			var angle = i * this.arcStep;
			var point = this.calculatePoint(angle, this.options.radius);
			this.paper.line(this.options.cx, this.options.cy, point.x, point.y).attr({'stroke-width': 1, stroke: '#666'});
		}
	},
	
	drawRadarPlots: function()
	{
		var idx = 0;
		this.series.each(function(s)
		{
			s.drawFill(this, idx++);
		}.bind(this));	
		
		idx = 0;
		this.series.each(function(s)
		{
			s.draw(this, idx);
			s.drawDots(this.idx++);
		}.bind(this));				
	},
	
	drawLabels: function()
	{
		this.labels.each(function(label, i)
		{
			var angle = i * this.arcStep;
			var point = this.calculatePoint(angle, this.options.radius + 20);
			this.paper.text(point.x, point.y, label).attr({'font-size': this.options.labelSize, "font-family": this.options.fontFamily, 'text-anchor': 'middle'});
		}.bind(this));
	},
	
//	drawGrid: function(x, y, w, h, wv, hv, color) 
//	{
//	    color = color || "#000";
//	    var path = ["M", Math.round(x) + .5, Math.round(y) + .5, "L", Math.round(x + w) + .5, Math.round(y) + .5, Math.round(x + w) + .5, Math.round(y + h) + .5, Math.round(x) + .5, Math.round(y + h) + .5, Math.round(x) + .5, Math.round(y) + .5],
//	        rowHeight = h / hv,
//	        columnWidth = w / wv;
//	    for (var i = 1; i < hv; i++) 
//	    {
//	        path = path.concat(["M", Math.round(x) + .5, Math.round(y + i * rowHeight) + .5, "H", Math.round(x + w) + .5]);
//	    }
//	    for (i = 1; i < wv; i++) 
//	    {
//	        path = path.concat(["M", Math.round(x + i * columnWidth) + .5, Math.round(y) + .5, "V", Math.round(y + h) + .5]);
//	    }
//	    return this.paper.path(path.join(" ")).attr({stroke: color});
//	},
	
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
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
	
	draw: function()
	{
		var chart = this.chart;
		var cmd = 'M';

		var path = [];

		this.series.values.each(function(val, i)
		{
			var angle = chart.arcStep * i;
			
			var magnitude = chart.options.radius * val / chart.max;
		
			var x = chart.options.cx - (Math.sin(angle) * magnitude);
			var y = chart.options.cy + (Math.cos(angle) * magnitude);
			path = chart.path(path, [cmd, x, y]);			
		});
		
		path = chart.path['z'];
		
		var lineColor = this.getColor(this.index);
		var f = this.series.options.areaFill ? lineColor : 'none';
		
		this.path = chart.paper.path(path).attr({"stroke-width": this.series.options.strokeWidth, stroke: lineColor, 
												 fill: f, 'fill-opacity': this.series.options.areaFillOpacity});
	}		
});

var SmoothedRadarSeriesRenderer = new Class({

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
		{});
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
		this.drawPlots();
		this.drawLabels();
		//this.drawTicks();
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

		this.arcStep = 2 * Math.PI / this.label.count;

		//this.axisRenderer.drawGrid(this.options.chartLeft, this.options.chartTop, this.options.chartWidth, this.options.chartHeight, count, this.options.ticks, this.palette.strokeColor);
	},
	
	drawRadarPlots: function()
	{
		var idx = 0;
		this.series.each(function(s)
		{
			s.draw(this, idx++);
		}.bind(this));			
	},
	
	drawLabels: function()
	{
		//this.axisRenderer.drawLabels();
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
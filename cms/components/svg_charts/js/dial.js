/**
 * 
 */
var Dial = new Class(
{
	Extends: 	Chart,
	Implements: [Options, Events],
	
	value: 	0,
	label: 	"",
	
	dial: null,
	sector: null,
	legend: null,
	
	targetSector: null,
	targetLegend: null,
	animation: null,
	
	options:
	{
		palette: 'standard',
		width: 600,
		height: 600,
		cx: 300,
		cy: 300,
		radius: 250,
		labelSize: 12,
		fontFamily: 'Arial',
		labelx: 250,
		labely: 300,
		strokeWidth: 2,
		fill: '#fff',
		animate: false,
		shadow: false,
		enableDownload: false,
		min: 0,
		max: 100,
		minColor: '#f00',
		maxColor: '#0f0',
		colorMode: 'fixed', // fixed or interpolated,
		showTarget: false,
		target: 0,
		targetLabel: ''
	},
	
	sectors: [],
	
	initialize: function(id, value, label, options)
	{
		this.parent(id);
		this.value = value;
		this.label = label;
		this.setOptions(options);
	},
	
	
	addShadow: function(shape)
	{
		if (!this.dropShadow)
		{
			this.dropShadow = this.paper.filter(Snap.filter.shadow(1, 1, 5, 0.1));
		}
		shape.attr({filter: this.dropShadow});
	},
	
	drawSector: function(value, color)
	{
		if (this.options.colorMode == 'interpolated')
		{
			color = this.interpolateColor(value);
		}
		
		var path = this.sectorPath(value);
        var params = { fill: color, stroke: color, "stroke-width": this.options.strokeWidth};

        var sector = this.paper.path({d: path}).attr(params);
        return sector;
	},
	
	drawChart: function()
	{
		this.createChart();
		
		this.dial = this.paper.circle(this.options.cx, this.options.cy, this.options.radius)
				  			  .attr({'stroke-width': this.options.strokeWidth, 
				  				  	 'fill': this.options.fill, 
				  				  	 'stroke': this.options.minColor});
		
		if (this.options.shadow)
		{
			this.addShadow(this.dial);
		}
		
		if (this.options.showTarget)
		{
			if (this.options.target > this.options.min)
			{
				this.targetSector = this.drawSector(this.options.target, this.options.maxColor);
			}
		}
		
        if (this.value > this.options.min)
        {
        	this.sector = this.drawSector(this.value, this.options.minColor);
        }
        
		if (this.label != '')
		{
			var color = this.options.minColor;
			if (this.options.colorMode == 'interpolated')
			{
				color = this.interpolateColor(this.value);
			}
			
			this.legend = this.paper.text(this.options.labelx, this.options.labely, this.label)
									.attr({fill: color, 
										   'stroke-width': 0, 
										   'font-family': this.options.fontFamily,
										   "font-size": this.options.labelSize,
										   "text-anchor": 'middle'});
		}
		
		this.dial.mouseover(function(event) 
		{
			this.displayTarget();
		}.bind(this));
		
		this.dial.mouseout(function(event)
		{
			this.hideTarget();
			
		}.bind(this));
	},
	
	displayTarget: function()
	{
		this.animation = Snap.animate(this.value, this.options.target, function(value)
		{
			this.sector.attr({d: this.sectorPath(value)});		
		}.bind(this), 500);
	},
	
	hideTarget: function()
	{
		this.animation = Snap.animate(this.options.target, this.value, function(value)
		{
			this.sector.attr({d: this.sectorPath(value)});		
		}.bind(this), 500);
	},
	
    sectorPath: function(value) 
	{
		var cx = this.options.cx;
		var cy = this.options.cy;
		var r  = this.options.radius - this.options.strokeWidth;
		
		if (value > this.options.max) value = this.options.max;
		if (value < this.options.min) value = this.options.min;
		
		var angle = 2 * Math.PI * (value / (this.options.max - this.options.min));
		
        var x1 = cx,
            x2 = cx + r * Math.sin(angle),
            y1 = cy - r,
            y2 = cy - r * Math.cos(angle);
        
        var sweep = (angle > Math.PI) ? 1:0;
        
        var path = this.path(['M', cx, cy], ['L', x1, y1], ['A', r, r, 0, sweep, 1, x2, y2], 'z');
        return path;
	},
	
	interpolateColor: function(value)
	{
		if (value > this.options.max) value = this.options.max;
		if (value < this.options.min) value = this.options.min;
		
		var ratio = value / (this.options.max - this.options.min);
		
		var minColor = Snap.color(this.options.minColor);
		var maxColor = Snap.color(this.options.maxColor);
		
		var range = {h: maxColor.h - minColor.h, s: maxColor.s - minColor.s, v: maxColor.v - minColor.v};
		var colorVals = {h: minColor.h + (ratio * range.h), s: minColor.s + (ratio * range.s), v: minColor.v + (ratio * range.v)};
		var color = Snap.hsb(colorVals.h, colorVals.s, colorVals.v);
		return color;
	}
});
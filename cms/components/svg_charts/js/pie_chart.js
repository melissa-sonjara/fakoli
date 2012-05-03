var PieChart = new Class(
{
	Extends: 	Chart,
	Implements: [Options],
	
	values: 	[],
	labels: 	[],
	
	percentages: [],
	
	options:
	{
		palette: 'standard',
		width: 600,
		height: 600,
		cx: 300,
		cy: 300,
		radius: 280,
		labelSize: 12,
		strokeWidth: 2,
		animate: true,
		shadow: false,
		percentages: true,
		percentagesSize: 16,
		percentagesDistance: 0.75,
		animatePercentages: true
	},
	
	sectors: [],
	
	initialize: function(id, options)
	{
		this.parent(id);
		this.setOptions(options);
	},
	
	draw: function()
	{
		this.createChart();
		
		var total = this.values.sum();
		var increment = 2 * Math.PI / total;
		var angle = 0;
		
		var cx = this.options.cx;
		var cy = this.options.cy;
		var r  = this.options.radius;
		
		this.values.each(function(val, idx) 
		{ 
			if (val == 0) return;
			
			var end = angle + val * increment; 
			var center = angle + (val * increment / 2);
			
			var s = this.sector(angle, end, idx);

			this.sectors.push(s);
			
			if (!this.options.legend)
			{
				var t = this.label(center, this.labels[idx]);
			}
			
			var p = Class.Empty;
			if (val != 0)
			{
				p = this.percentage(center, val * 100 / total);
			}
			var animatePercentages = this.options.animatePercentages;
			
			if (this.options.animate)
			{
				s.mouseover(function () 
				{
	                s.stop().animate({transform: "s1.1 1.1 " + cx + " " + cy}, 500, "elastic");
	                if (p && animatePercentages) p.stop().animate({opacity: 1}, 500, "elastic");
	            });
				
				s.mouseout(function () 
				{
	                s.stop().animate({transform: ""}, 500, "elastic");
	                if (p && animatePercentages) p.stop().animate({opacity: 0}, 500, "elastic");
	            });				
			}
			
			angle = end; 
		
		}.bind(this));
		
		if (this.options.emboss)
		{
			this.sectors.each(function(e) {e.emboss(); });
		}
		
		if (this.options.shadow)
		{
			this.sectors.each(function(e) { e.dropShadow(5, 1, 1, 0.2); });
		}
		
		this.drawLegend();
	},
	
    sector: function(startAngle, endAngle, index) 
	{
		var cx = this.options.cx;
		var cy = this.options.cy;
		var r  = this.options.radius;
		
        var x1 = cx + r * Math.cos(-startAngle),
            x2 = cx + r * Math.cos(-endAngle),
            y1 = cy + r * Math.sin(-startAngle),
            y2 = cy + r * Math.sin(-endAngle);

        var params = { fill: this.palette.swatches[index], stroke: this.palette.strokeColor, "stroke-width": this.options.strokeWidth};

        return this.paper.path(["M", cx, cy, "L", x1, y1, "A", r, r, 0, +(endAngle - startAngle > Math.PI), 0, x2, y2, "z"]).attr(params);
	},
	
	label: function (angle, text)
	{
		var cx = this.options.cx;
		var cy = this.options.cy;
		var r  = this.options.radius;

		var params = {fill: this.palette.strokeColor, stroke: "none" , opacity: 1, "font-size": this.options.labelSize};
		var t = this.paper.text(cx + (r * 1.2) * Math.cos(-angle), cy + (r * 1.2) * Math.sin(-angle), text).attr(params);
		return t;
	},
	
	percentage: function(angle, value)
	{
		var cx = this.options.cx;
		var cy = this.options.cy;
		var r  = this.options.radius;
	
		var text = value.toFixed(1) + "%";
		var o = this.options.animatePercentages ? 0 : 1;
		
		var params = {fill: this.palette.strokeColor, stroke: "none" , opacity: o, "font-size": this.options.percentagesSize};
		var t = this.paper.text(cx + (r * this.options.percentagesDistance) * Math.cos(-angle), cy + (r * this.options.percentagesDistance) * Math.sin(-angle), text).attr(params);
		
		return t;
	}
	
});
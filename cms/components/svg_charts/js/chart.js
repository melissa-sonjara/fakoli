var Chart = new Class(
{
	id:			"",
	container:	Class.Empty,
	paper: 		Class.Empty,
	palette:	Class.Empty,
	
	initialize: function(id)
	{
		this.id = id;
		this.container = document.id(id);
		this.container.chart = this;
	},
	
	createChart: function()
	{
		var ratio = this.options.height / this.options.width;
		var size = this.container.getSize();
		if (size.y == 0)
		{
			size.y = size.x * ratio;
			this.container.set('height', size.y);
			this.container.addEvent('size', function(e)
			{
				var size = this.container.getSize();
				size.y = size.x * ratio;
				this.container.set('height', size.y);
				this.paper.setSize(size.x, size.y);
			});
		}

		this.paper = Raphael(this.container.id, size.x, size.y);
		this.paper.setViewBox(0, 0, this.options.width, this.options.height, true);
		this.palette = Palette.palettes[this.options.palette];
	},
	
	draw: function()
	{
		if (!this.testSVG())
		{
			
		}
		
		this.drawChart();		
	},
	
	drawLegend: function()
	{
		if (this.options.legend)
		{
			var x = this.options.legendX;
			var y = this.options.legendY;
			
			this.labels.each(function(text, index)
			{
				var rect = this.paper.rect(x, y, 20, 20, 3);
				rect.attr({fill:this.palette.swatches[index], stroke: this.palette.strokeColor, "stroke-width": this.options.strokeWidth});
				
				var text = this.paper.text(x + 30, y + 10, text);
				text.attr({"text-anchor": "start", fill: this.palette.strokeColor, stroke: "none" , opacity: 1, "font-size": this.options.labelSize});
							
				y+=30;
			}.bind(this));
		}
	},
	
	testSVG: function() 
	{
	    return !!document.createElementNS && !! document.createElementNS('http://www.w3.org/2000/svg', 'svg').createSVGRect;
	}
});
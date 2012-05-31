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
		this.paper = Raphael(this.container.id, this.options.width, this.options.height);
		this.palette = Palette.palettes[this.options.palette];
	},
	
	draw: function()
	{
		this.drawChart();
		
	    //Handle Android
	    if (!this.testSVG()) 
	    {
	        //Get chart SVG and output to canvas
	        var canvas = document.createElement("canvas", {'id': 'canvas_' + this.id});
	        canvas.setAttribute("style", "height:" + chartEle.height() + ";width:" + chartEle.width() + ";");

	        canvg(canvas.id, this.container.get('html'));

	        this.container.empty(); //NOTE: Android 2.1 is hit and miss here
	        this.container.insert(canvas);
	    }
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
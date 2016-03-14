var MorphingHistogram = new Class({

	Extends: Histogram,
	
	nextButton: Class.Empty,
	prevButton: Class.Empty,
	
	columns: [],
	titles: [],
	
	initialize: function(id, options, labels)
	{
		this.options.startIndex = 0;
		this.options.enableDownload = false;
		
		this.parent(id, options, labels);
	},
	
	drawChart: function()
	{
		this.createChart();

		this.setupHistogram();
		
		this.index = this.options.startIndex;
		this.blockWidth = this.columnWidth * (1 - this.options.columnMargin);
		
		this.drawHistogram();
		
        
		var prevleft = this.options.chartLeft + 8;
		
        this.prevButton = this.paper.circle(prevleft, 20, 16).attr({fill: this.palette.buttonColor, stroke: "none", 'cursor': 'pointer'}).dropShadow(2, 1, 1, 0.1);
        this.prevArrow = this.paper.path("M" + (prevleft + 5) + ",14l-12,6 12,6z").attr({fill: this.palette.strokeColor, 'cursor': 'pointer'});

        var nextLeft = this.options.chartLeft + this.options.chartWidth - 12;

        this.nextButton = this.paper.circle(nextLeft, 20, 16).attr({fill: this.palette.buttonColor, stroke: "none", 'cursor': 'pointer'}).dropShadow(2, 1, 1, 0.1);
        this.nextArrow = this.paper.path("M" + (nextLeft - 4) + ",14l12,6 -12,6z").attr({fill: this.palette.strokeColor, 'cursor': 'pointer'});

        this.prevButton.click(function() { this.prev(); }.bind(this))
        			   .mouseover(function() { this.highlight(this.prevButton, this.prevArrow); }.bind(this))
        			   .mouseout(function() { this.unhighlight(this.prevButton, this.prevArrow); }.bind(this));
        
        this.prevArrow.click(function() { this.prev(); }.bind(this))
		   			  .mouseover(function() { this.highlight(this.prevButton, this.prevArrow); }.bind(this))
		   			  .mouseout(function() { this.unhighlight(this.prevButton, this.prevArrow); }.bind(this));

        this.nextButton.click(function() { this.next(); }.bind(this))
		   			   .mouseover(function() { this.highlight(this.nextButton, this.nextArrow); }.bind(this))
		   			   .mouseout(function() { this.unhighlight(this.nextButton, this.nextArrow); }.bind(this));
        
        this.nextArrow.click(function() { this.next(); }.bind(this))
        			  .mouseover(function() { this.highlight(this.nextButton, this.nextArrow); }.bind(this))
        			  .mouseout(function() { this.unhighlight(this.nextButton, this.nextArrow); }.bind(this));;
        
        this.series.each(function(s, i)
   		{
        	this.titles[i] = this.paper.text(this.options.chartWidth / 2 + this.options.chartLeft, this.options.chartTop - 30, s.title);
        	this.titles[i].attr({stroke: 'none', fill: this.palette.strokeColor, "font-size": this.options.titleSize, "opacity": i == this.index ? 1 : 0});
   		}.bind(this));
	},
	
	drawBlocks: function()
	{
		this.series[this.index].drawFill(this, 0);
		this.series[this.index].draw(this, 0);
		this.columns = this.series[this.index].columns;
	},
	
	drawLegend: function()
	{
		if (this.options.legend && this.series.length >0)
		{
			var series = this.series[0];
			if (typeof this.series[0] != 'GroupedHistogramSeries') return;
			
			var x = this.options.legendX;
			var y = this.options.legendY;
			var s = this.options.legendSwatchSize || 20;
			var h = this.options.legendLineHeight || 30;
			
			series.children.each(function(series, index)
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
	},
	
	next: function()
	{
		if (this.index < this.series.length - 1)
		{
			this.titles[this.index].animate({'opacity': 0}, 1000, '<>');
			this.index++;
			this.titles[this.index].animate({'opacity': 1}, 1000, '<>');
			this.morphColumns();
		}
	},
	
	prev: function()
	{
		if (this.index > 0)
		{
			this.titles[this.index].animate({'opacity': 0}, 1000, '<>');
			this.index--;
			this.titles[this.index].animate({'opacity': 1}, 1000, '<>');
			this.morphColumns();
		}
	},
	
	morphTo: function(idx)
	{
		if (this.index == idx) return;
		
		this.titles[this.index].animate({'opacity': 0}, 1000, '<>');
		this.index = idx;
		this.titles[this.index].animate({'opacity': 1}, 1000, '<>');
		this.morphColumns();
	},
		
	morphColumns: function()
	{
		this.series[this.options.startIndex].morph(this.series[this.index]);
	},
	
	highlight: function(button, arrow)
	{
		button.attr({'fill': this.palette.strokeColor});
		arrow.attr({'fill': this.palette.buttonColor, 'stroke': this.palette.buttonColor});
	},
	
	unhighlight: function(button, arrow)
	{
		button.attr({'fill': this.palette.buttonColor});
		arrow.attr({'fill': this.palette.strokeColor, 'stroke': this.palette.strokeColor});
	}
});
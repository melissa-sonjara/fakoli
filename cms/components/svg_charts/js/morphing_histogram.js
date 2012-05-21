var MorphingHistogram = new Class({

	Extends: Histogram,
	
	nextButton: Class.Empty,
	prevButton: Class.Empty,
	
	columns: [],
	titles: [],
	
	initialize: function(id, options, labels)
	{
		this.options.startIndex = 0;
		
		this.parent(id, options, labels);
	},
	
	draw: function()
	{
		this.createChart();

		this.setupHistogram();
		
		this.index = this.options.startIndex;
		this.blockWidth = this.columnWidth * (1 - this.options.columnMargin);
		
		this.drawChart();
		
        
		var prevleft = this.options.chartLeft + 8;
		
        this.prevButton = this.paper.circle(prevleft, 20, 16).attr({fill: this.palette.buttonColor, stroke: "none"}).dropShadow(2, 1, 1, 0.1);
        this.prevArrow = this.paper.path("M" + (prevleft + 5) + ",14l-12,6 12,6z").attr({fill: this.palette.strokeColor});

        var nextLeft = this.options.chartLeft + this.options.chartWidth - 12;

        this.nextButton = this.paper.circle(nextLeft, 20, 16).attr({fill: this.palette.buttonColor, stroke: "none"}).dropShadow(2, 1, 1, 0.1);
        this.nextArrow = this.paper.path("M" + (nextLeft - 4) + ",14l12,6 -12,6z").attr({fill: this.palette.strokeColor});

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
		this.series[this.index].draw(this, 0);
		this.columns = this.series[this.index].columns;
	},
	
	next: function()
	{
		if (this.index < this.blockSeriesCount - 1)
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
	
	morphColumns: function()
	{
		this.series[0].morph(this.series[this.index]);
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
var PieChart = new Class(
{
	Extends: 	Chart,
	Implements: [Options, Events],
	
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
		fontFamily: 'Arial',
		strokeWidth: 2,
		animate: true,
		shadow: false,
		percentages: true,
		percentagesSize: 16,
		percentagesDistance: 0.75,
		animatePercentages: true,
		onSectorOver: Class.Empty,
		onSectorOut: Class.Empty,
		onSectorClick: Class.Empty,
		onSelectionChanged: Class.Empty,
		enableDownload: true,
		selectable: false,
		animateSelection: false,
		preselected: [],
		title: "",
		titlex: 300,
		titley: 25,
		titleSize: 14
	},
	
	sectors: [],
	
	initialize: function(id, options)
	{
		this.parent(id);
		this.setOptions(options);
		
		if (this.options.selectable)
		{
			this.addEvent('legendClick', function(e, idx)
			{
				this.sectorSelect(idx);
			}.bind(this));
		}

	},
	
	drawChart: function()
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
			if (val == 0) 
			{
				this.sectors.push(null);
				return;
			}
			
			var end = angle + val * increment; 
			var center = angle + (val * increment / 2);
			
			var s = this.sector(angle, end, idx);

			this.sectors.push(s);
			
			angle = end; 
		}.bind(this));
	
		angle = 0;

		this.values.each(function(val, idx)
		{
			var end = angle + val * increment; 
			var center = angle + (val * increment / 2);
			
			var s = this.sectors[idx];
			if (!this.options.legend)
			{
				var t = this.label(center, this.labels[idx], idx);
			}
			
			var p = Class.Empty;
			if (val != 0 && this.options.percentages)
			{
				p = this.percentage(center, val * 100 / total, idx);
			}
			var animatePercentages = this.options.animatePercentages;
			
			if (this.options.animate && val != 0)
			{
				s.mouseover(function (event) 
				{
					if (!s.hasClass('selected') || !this.options.animateSelection)
					{
		                s.stop().animate({transform: "s1.1 1.1 " + cx + " " + cy}, 500, mina.elastic);
		                if (p && animatePercentages) p.stop().animate({opacity: 1}, 500, mina.elastic);
					}
	                this.fireEvent('sectorOver', [event, idx]);
	            }.bind(this));
				
				s.mousemove(function(event) { this.fireEvent('sectorOver', [event, idx]); }.bind(this));
				
				s.mouseout(function (event) 
				{
					if (!s.hasClass('selected') || !this.options.animateSelection)
					{
						s.stop().animate({transform: ""}, 500, mina.elastic);
						if (p && animatePercentages) p.stop().animate({opacity: 0}, 500, mina.elastic);
					}
	                this.fireEvent('sectorOut', [event, idx]);
	            }.bind(this));		
				
				s.click(function(event) { this.sectorSelect(idx); }.bind(this));
				
				if (this.$events.sectorClick)
				{
					s.attr({cursor: "pointer"});
					s.click(function(event) { this.fireEvent('sectorClick', [event, idx]); }.bind(this));
				}
			}
			
			angle = end; 
		
		}.bind(this));

		this.selectSectors(this.options.preselected);
		
		if (this.options.emboss)
		{
			//this.sectors.each(function(e) {e.emboss(); });
		}
		
		if (this.options.shadow)
		{
			this.sectors.each(function(e) { e.dropShadow(5, 1, 1, 0.2); });
		}
		
		if (this.options.title)
		{
			this.paper.multitext(this.options.titlex, this.options.titley, this.options.title, this.options.width, 
								 {fill: this.palette.strokeColor, stroke: "none" , opacity: 1, "font-size": this.options.titleSize, "font-family": this.options.fontFamily, "text-anchor": "middle"});
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

        if (this.options.selectable)
        {
        	params["class"] = "selectable";
        }
        
        if (Math.abs(x1 - x2) < 1 && Math.abs(y1 - y2) < 1 && Math.round(startAngle) != Math.round(endAngle))
        {
        	return this.paper.circle(cx, cy, r).attr(params);
        }
        
        var sector = this.path(['M', cx, cy], ['L', x1, y1], ['A', r, r, 0, +(endAngle - startAngle > Math.PI), 0, x2, y2], 'z');
        return this.paper.path(sector).attr(params);
	},
	
	label: function (angle, text, idx)
	{
		var cx = this.options.cx;
		var cy = this.options.cy;
		var r  = this.options.radius;

		var color = this.palette.getFontColor(idx);
		
		var params = {fill: color, stroke: "none" , opacity: 1, "font-size": this.options.labelSize, "font-family": this.options.fontFamily};
		var t = this.paper.text(cx + (r * 1.2) * Math.cos(-angle), cy + (r * 1.2) * Math.sin(-angle), text).attr(params);
		return t;
	},
	
	percentage: function(angle, value, idx)
	{
		var cx = this.options.cx;
		var cy = this.options.cy;
		var r  = this.options.radius;
	
		var text = value.toFixed(1) + "%";
		var o = this.options.animatePercentages ? 0 : 1;

		var color = this.palette.getFontColor(idx);
		
		var params = {fill: color, stroke: "none" , opacity: o, "font-size": this.options.percentagesSize, "font-family": this.options.fontFamily, "text-anchor": "middle"};
		var t = this.paper.text(cx + (r * this.options.percentagesDistance) * Math.cos(-angle), cy + (r * this.options.percentagesDistance) * Math.sin(-angle), text).attr(params);
		
		return t;
	},
	
	addSectorClass: function(idx, cl)
	{
		var sector = this.sectors[idx];
		if (sector)
		{
			sector.addClass(cl);
		}
	},
	
	removeSectorClass: function(idx, cl)
	{
		var sector = this.sectors[idx];
		if (sector)
		{
			sector.removeClass(cl);
		}
	},
	
	disableSector: function(idx)
	{
		this.addSectorClass(idx, 'disabled');
	},
	
	enableSector: function(idx)
	{
		this.removeSectorClass(idx, 'disabled');
	},
	
	selectSector: function(idx)
	{
		this.addSectorClass(idx, 'selected');		
		this.showSelected();
	},
	
	selectSectors: function(indices)
	{
		indices.each(function(idx) { this.addSectorClass(idx, 'selected');}.bind(this) );
		this.showSelected();
	},
	
	deselectSector: function(idx)
	{
		this.removeSectorClass(idx, 'selected');		
		this.showSelected();
	},
	
	clearSelection: function()
	{
		this.sectors.each(function(sector)
		{
			if (sector != null) sector.removeClass('selected');
		});
		
		this.showSelected();
	},
	
	sectorSelect: function(idx)
	{
		if (!this.options.selectable)
		{
			return;
		}
		
		var sector = this.sectors[idx];
		
		if (sector != null)
		{
			if (sector.hasClass('selected'))
			{
				sector.removeClass('selected');
			}
			else
			{
				sector.addClass('selected');
			}
		}
		
		this.showSelected();
		this.fireEvent('selectionChanged', [this, idx]);
	},
	
	showSelected: function()
	{
		var cx = this.options.cx;
		var cy = this.options.cy;
	
		if (this.options.animateSelection)
		{
			this.sectors.each(function(s)
			{
				if (s == null) return;
				if (s.hasClass('selected'))
				{
	                s.stop().animate({transform: "s1.15 1.15 " + cx + " " + cy}, 500, mina.elastic);	
				}
				else
				{
					s.stop().animate({transform: ""}, 500, mina.elastic);	
				}
			});
		}
		else
		{
			this.sectors.each(function(s)
			{
				if (s == null) return;
				if (s.hasClass('selected'))
				{
					s.attr({transform: "s1.15 1.15 " + cx + " " + cy});
				}
				else
				{
					s.attr({transform: ""});
				}
			});
		}
	}
});
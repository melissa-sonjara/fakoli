var ImageGrid = new Class({
	
	Implements: Options,
	grid: Class.Empty,
	size: 200,
	options:
	{
		puff: true,		
	},
	
	initialize: function(grid, size, options)
	{
		var me = this;
		this.setOptions(this);
		
		this.size = size;
		this.grid = document.id(grid);
		
		if (!this.grid) return;
		
		if (this.options.puff)
		{
			this.grid.getElements('li').each(function(elt)
			{
				elt.addEvent('mouseover', function(e) { new DOMEvent(e).stop(); this.puff(elt); }.bind(me));
				elt.addEvent('mouseout', function(e) { new DOMEvent(e).stop(); this.unpuff(elt); }.bind(me));
			});
		}
		
		if (this.grid.facetManager)
		{
			this.grid.facetManager.addEvent('filterChanged', function() { this.filterChanged()}.bind(this));
			this.grid.facetManager.addEvent('filterCleared', function() { this.filterCleared()}.bind(this));
			this.preprocessFacets();
		}
	},
	
	puff: function(elt)
	{
		div = elt.getElement('div');
		img = elt.getElement('img');
		elt.setStyles({'width': this.size + 30, 'height': this.size + 60, margin: 0});
		div.setStyles({'width': this.size + 20, 'height': this.size + 60});
		img.setStyles({'width': this.size + 20, height: this.size + 20});
	},
	
	unpuff: function(elt)
	{	
		div = elt.getElement('div');
		img = elt.getElement('img');
		elt.setStyles({'width': this.size + 10, 'height': this.size + 40, margin: 10});
		div.setStyles({'width': this.size, 'height': this.size + 40});
		img.setStyles({'width': this.size, height: this.size});
	},
		
	preprocessFacets: function()
	{
		this.grid.getElements('li').each(function(elt)
		{
			this.grid.facetManager.preprocess(elt);
		}.bind(this));
	},
	
	filterChanged: function()
	{
		this.grid.getElements('li').each(function(elt)
		{
			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
			
			var match = this.grid.facetManager.filter(elt);
			
			if (match)
			{
				elt.addClass('filtermatch');
				elt.set('tween', {property: 'opacity'});
				elt.setStyle('display', 'block');
				elt.get('tween').start(1);
			}
			else
			{
				elt.addClass('filtered');
				elt.set('tween', {property: 'opacity'});
				elt.get('tween').start(0).chain(function() {this.element.setStyle('display', 'none'); });
			}
		}.bind(this));
	},
	
	filterCleared: function()
	{
		this.grid.getElements('li').each(function(elt)
		{
			elt.removeClass("filtered");
			elt.removeClass("filtermatch");
			elt.set('tween', {property: 'opacity'});
			elt.setStyle('display', 'block');
			elt.get('tween').start(1);
		});
	}
	
});
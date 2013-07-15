var Sproing = new Class(
{
	Implements: [Options],
	    
	options: 
	{
		margin: 0,
		width: 960,
		in_image: "",
		in_hover: "",
		out_image: "",
		out_hover: "",
		sproinged: false,
		additional: []
	},
		
	element: Class.Empty,
	button:  Class.Empty,
	subElements: [],
	
	initialize: function(element, button, options)
	{
		this.element = $(element);
		this.button = $(button);
		this.setOptions(options);
		
		this.button.addEvent('click', function() { this.toggleSproing(); }.bind(this));
		this.button.addEvent('mouseover', function() { this.overSproinger();}.bind(this));
		this.button.addEvent('mouseout', function() { this.outSproinger(); }.bind(this));
		
		this.options.additional.each(function(elt)
		{
			elt = document.id(elt);
			w = elt.getWidth();
			
			this.subElements.push({difference: this.options.width - w, element: elt});
		}.bind(this));		
	},
	
	toggleSproing: function()
	{
		if (this.options.sproinged)
		{
			new Fx.Tween(this.element, {transition: Fx.Transitions.Elastic.easeOut}).start("width", this.options.width);
			
			this.subElements.each(function(sub)
			{				
				new Fx.Tween(sub.element, {transition: Fx.Transitions.Elastic.easeOut}).start("width", this.options.width - sub.difference);
			}.bind(this));
			
			this.element.setStyles({'margin-left': 'auto', 'margin-right': 'auto'});
			this.options.sproinged = false;
			this.button.src = this.options.in_image;
		}
		else
		{
			var w = (window.innerWidth || document.documentElement.clientWidth) - (this.options.margin * 2);
			new Fx.Tween(this.element, {transition: Fx.Transitions.Elastic.easeOut}).start("width", w);
			
			this.subElements.each(function(sub)
			{				
				new Fx.Tween(sub.element, {transition: Fx.Transitions.Elastic.easeOut}).start("width", w - sub.difference);
			}.bind(this));

//			this.element.setStyles({'width': 'auto', 'margin-left': this.options.margin, 'margin-right': this.options.margin});
			this.options.sproinged = true;
			this.button.src = this.options.out_image;
		}
	},
	
	overSproinger: function()
	{
		this.button.src = (this.options.sproinged) ? this.options.out_hover: this.options.in_hover;
	},
	
	outSproinger: function()
	{
		this.button.src = (this.options.sproinged) ? this.options.out_image: this.options.in_image;
	}
		
});
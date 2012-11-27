var SubSelectManager = new Class(
{
	select: Class.Empty,
	subSelect: Class.Empty,
	subSelectLabel: Class.Empty,
	value: 0,
	selectOptions: {},
	
	initialize: function(select, subSelect, value)
	{
		this.select = document.id(select);
		this.subSelect = document.id(subSelect);
		this.subSelectLabel = document.id(subSelect + "_label");
		
		this.value = value;
		
		this.select.addEvent('change', function() { this.update(true); }.bind(this));
		this.subSelect.addEvent('change', function () { this.value = this.subSelect.value;}.bind(this));
		this.update();
	},
	
	clear: function()
	{
		this.subSelect.set('html', '');
	},
	
	update: function(fade)
	{
		this.clear();
		var val = this.select.value;
		var opts = this.selectOptions[val];
		
		if (typeof opts === 'undefined' || !isArray(opts))
		{
			this.subSelect.setStyle('display', 'none');
			this.subSelectLabel.setStyle('display', 'none');
			return;
		}
		
		opts.each(function(opt)
		{
			var option = new Element('option', {'text': opt.name, 'value': opt.value, 'selected': opt.value == this.value});
			option.inject(this.subSelect, 'bottom');
		}.bind(this));
		
		if (fade)
		{
			this.subSelect.setStyles({'display': 'inline', 'opacity': 0});
			this.subSelectLabel.setStyles({'display': 'inline', 'opacity': 0});
			this.subSelect.fade('in');
			this.subSelectLabel.fade('in');
		}
		else
		{
			this.subSelect.setStyles({'display': 'inline', 'opacity': 1});
			this.subSelectLabel.setStyles({'display': 'inline', 'opacity': 1});			
		}
	},
	
	addOptions: function(selectValue, options)
	{
		this.selectOptions[selectValue] = options;
	}
});
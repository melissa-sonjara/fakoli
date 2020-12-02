var SubSelectManager = new Class(
{
	select: Class.Empty,
	subSelect: Class.Empty,
	subSelectLabel: Class.Empty,
	subSelectVisibility: Class.Empty,
	value: 0,
	selectOptions: {},
	
	initialize: function(select, subSelect, value)
	{
		this.select = document.id(select);
		this.subSelect = document.id(subSelect);
		this.subSelectLabel = document.id(subSelect + "_label");
		
		this.select.manager = this;
		
		var container = this.subSelect.getParent();
		
		this.subselectVisibility = container.hasClass('subselect_container') ? container : this.subSelect;
		
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
			this.subselectVisibility.setStyle('display', 'none');
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
			this.subselectVisibility.setStyles({'display': 'inline', 'opacity': 0});
			this.subSelectLabel.setStyles({'display': 'inline', 'opacity': 0});
			this.subselectVisibility.fade('in');
			this.subSelectLabel.fade('in');
		}
		else
		{
			this.subselectVisibility.setStyles({'display': 'inline', 'opacity': 1});
			this.subSelectLabel.setStyles({'display': 'inline', 'opacity': 1});			
		}
	},
	
	addOptions: function(selectValue, options)
	{
		this.selectOptions[selectValue] = options;
	},
	
	validateRequired: function()
	{
		var selected = this.select.value;
		var subSelected = this.subSelect.value;
		
		
		if (!selected) return false;

		var opts = this.selectOptions[selected];
		
		if ((!subSelected || subSelected === "0") && (typeof opts !== 'undefined')) return false;
		return true;
	}
});
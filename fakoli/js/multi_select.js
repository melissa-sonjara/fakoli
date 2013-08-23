var MultiSelect = new Class(
{
	Implements: [Options, Events],
	
	container: Class.Empty,
	dropdown:  Class.Empty,
	
	options:
	{
		message:  "Click to Select",
		maxWidth: "400px",
		onSelectionChanged: function() {}
	},
	
	initialize: function(container, options)
	{
		this.container = document.id(container);
		this.setOptions(options);
		
		this.container.multiSelect = this; // back reference
		
		this.buildDropdown();
		this.selectionChanged();
	},
		
	buildDropdown: function()
	{
		this.dropdown = new Element('a', 
		{
			'class': 'multi_select_dropdown',
			'href': '#',
			'text': this.options.message
		});
		
		this.dropdown.setStyle("max-width", this.options.maxWidth);
		
		this.dropdown.inject(this.container, 'before');


		this.container.setStyles({display: 'none', opacity: 0});
		
		this.container.addEvent('mouseleave', function(e) { this.container.fade('out'); }.bind(this));
			
		this.dropdown.addEvent('click', function(e)
		{
			new Event(e).stop();
			if (this.container.getStyle('display') == 'none' || this.container.getStyle('opacity') == 0)
			{
				$$('.multi_select_dropdown_list').each(function(elt) { elt.setStyle('display', 'none');});
				this.container.setStyles({display: 'block', opacity: 0});
				this.container.position({'relativeTo': this.dropdown, 'position': 'bottomLeft', 'edge': 'topLeft'});
				this.container.fade('in');
			}
			else
			{
				this.container.fade('out');
			}
		}.bind(this));

		this.connectCheckboxes();
	},
	
	connectCheckboxes: function()
	{		
		this.container.getElements('input[type=checkbox]').each(function(cbox)
		{
			cbox.addEvent('click', function(e) { this.selectionChanged(); }.bind(this));
			label = cbox.getNext('label');
//			label.removeEvents('click');
//			label.addEvent('click', function(e) 
//			{
//				cbox.click();
//			});
		}.bind(this));	
	},
	
	getCheckboxes: function()
	{
		return this.container.getElements("input[type=checkbox]");
		
	},
	
	addCheckbox: function(text, value)
	{
		var id = "cbox_" + String.uniqueID();
		
		var cbox = new Element('input', {'type': 'checkbox', 'id': id, 'value': value});
		var label = new Element('label', {'for': id});

		cbox.inject(label, 'top');
		label.inject(this.container);
		label.appendText(text);
		
		cbox.addEvent('click', function(e) { this.selectionChanged();}.bind(this));
		//label.removeEvents('click');
		//label.addEvent('click', function(e) 
		//{
		//	cbox.click();
		//});
	},
	
	selectionChanged: function()
	{
		var selected = this.getCheckboxes();
		
		var message = [];
		var self = this;
		selected.each(function(cbox)
		{
			if (cbox.checked)
			{
				var label = self.container.getElement('label[for="' + cbox.id + '"]');
				if (label)
					message.push(label.get('text'));
			}
		});

		if (message.length == 0)
		{
			this.dropdown.set('text', this.options.message);
		}
		else
		{
			this.dropdown.set('text', message.join(", "));			
		}
		
		this.fireEvent("selectionChanged");
	}
});


/**
 * 
 */
var SectionContentPropertiesManager = new Class({
	
	id:		null,
	ctrl: 	null,
	table: 	null,
	data:	null,
	identifier: '',
	
	initialize: function(id, identifier)
	{
		this.id = id;
		this.ctrl = document.id(id);
		this.table = document.id(id + "_table");
		
		this.identifier = identifier;
		this.data = JSON.decode(this.ctrl.value);
		
		this.bindControls();
	},
	
	bindControls: function()
	{
		this.table.getElements('tr[data-section_id]').each(function(tr)
		{
			var section_id = tr.get('data-section_id');
			var cbox = tr.getElement("input[type='checkbox']");
			var button = tr.getElement('a.button');
			
			if (this.data[section_id])
			{
				cbox.checked = true;
			}
			
			button.addEvent('click', function(e) { new DOMEvent(e).stop(); this.showPropertiesDialog(section_id); return false; }.bind(this));
			
		}.bind(this));
	},

	updateIdentifier: function(identifier)
	{
		this.identifier = identifier;
		this.updateValue();
	},
	
	updateValue: function()
	{
		var flattened = [];

		Object.keys(this.data).each(function(key) 
		{
			this.data[key].identifier = this.identifier;
			flattened.push(this.data[key]);
		}.bind(this));
		this.ctrl.value = JSON.stringify(flattened);
	},
	
	showPropertiesDialog: function(section_id)
	{
		var data;
		
		if (this.data[section_id])
		{
			data = this.data[section_id];
		}
		else
		{
			data = {'section_id': section_id, 'identifier': this.identifier};
		}
		
		var json = encodeURIComponent(JSON.stringify(data));
		
		this.dialog = modalPopup("Content Properties", "/action/section/content_properties_json?data=" + json, '650px', 'auto', true);		

	},
	
	propertiesChanged: function(response)
	{
		var data = JSON.parse(response);
		if (data.result)
		{
			var section_id = data.result.section_id;
			
			if (section_id)
			{
				this.data[section_id] = data.result;
			}
		}
		
		this.updateValue();

		this.dialog.hide();
		this.dialog = null;
	},
	
	cancel: function()
	{
		this.dialog.hide();
		this.dialog = null;
	}
});
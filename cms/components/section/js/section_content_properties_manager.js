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
			
			button.addEvent('click', function(e) { new DOMEvent(e).stop(); this.showPropertiesDialog(section_id); return false; });
			
		}.bind(this));
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
	}
});
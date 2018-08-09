/**
 * SubSelectChecklistManager
 */
var SubSelectChecklistManager = new Class({
	
	select: 		Class.Empty,
	subSelect: 		Class.Empty,
	
	initialize: function(select, subSelect)
	{
		this.select			= document.id(select);
		this.subSelect		= document.id(subSelect);
		
		this.select.addEvent('change', function() { this.update(); }.bind(this));
		this.update();
	},
	
	update: function()
	{
		
		var value = this.select.value;
		
		this.subSelect.getElements('[data-select]').each(function(item)
		{
			var input = item.getElement('input');
			if (item.get('data-select') == value)
			{
				item.setStyle('display', 'block');
			}
			else
			{
				item.setStyle('display', 'none');
				input.checked = false;
			}
		});
	}
});
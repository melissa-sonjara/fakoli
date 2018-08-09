/**
 * SubSelectChecklistManager
 */
var SubSelectChecklistManager = new Class({
	
	select: 		Class.Empty,
	subSelect: 		Class.Empty,
	subSelectValue: Class.Empty,
	
	initialize: function(select, subSelect, subSelectValue)
	{
		this.select			= document.id(select);
		this.subSelect		= document.id(subSelect);
		this.subSelectValue = document.id(subSelectValue);
		
		this.select.addEvent('change', function() { this.update(); }.bind(this));
		this.update();
	},
	
	update: function()
	{
		
		var value = this.select.value;
		var checked = [];
		
		this.subselect.getElements('[data-select]').each(function(item)
		{
			var input = item.getElement('input');
			if (item.get('data-select') == value)
			{
				item.setStyle('display', 'block');
				if (input.checked) checked.push(input.value);
			}
			else
			{
				item.setStyle('display', 'none');
				input.checked = false;
			}
		});
		
		this.subSelectValue = checked.join(',');
	}
});
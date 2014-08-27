var TextLookup =  (function()
{
	var TextLookupSingleton = new Class(
	{
		dialog: null,
			
		initialize: function()
		{
		},
		
		closeDialog: function()
		{
			this.dialog.hide();
		},
		
		showAdvancedFeatures: function()
		{
			var class_name = document.id('class_name').value;
			this.dialog = modalPopup('Advanced Features', '/action/text_lookup/advanced_features?class_name=' + class_name, '500px', 'auto');	
		}
		
		
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new TextLookupSingleton();
	};
	
})();

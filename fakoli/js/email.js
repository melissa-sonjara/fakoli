var EmailManager =  (function()
{
	var EmailManagerSingleton = new Class(
	{
		dialog: null,
			
		initialize: function()
		{
		},
		
		sendingClassListDialog: function(response)
		{
			
		},
		
		closeClassListDialog: function()
		{
			this.dialog.hide();
		},
		
		openClassListDialog: function()
		{
			this.dialog = modalPopup('Select Sending Class', '/action/email/auto_create_class_list', '450px', 'auto', true);
		},
			
		createMergeCode: function(relation, field)
		{
			var map = $('MergeCode_form_map');
			var name = $('MergeCode_form_name');
			var description = $('MergeCode_form_description');
			
			if(map)
				map.value = relation + '.' + field;
			
			if(name)
				name.value = relation.toLowerCase() + '_' + field; 
				
			if(description)
				description.set('text', '');
			
			this.closeMergeCodeDialog();
		},
		
		closeMergeCodeDialog: function()
		{
			this.dialog.hide();
		},
		
		
		openMergeCodeDialog: function(class_name)
		{
			this.dialog = modalPopup('AutoCreate Merge Code', '/action/email/auto_create_merge_code?class_name=' + class_name, '400px', 'auto', true);
		},
		
		showAdvancedFeatures: function()
		{
			var class_name = $('class_name').value;
			this.dialog = modalPopup('Advanced Features', '/action/email/advanced_email_features?class_name=' + class_name, '500px', 'auto')	
		}
		
		  
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new EmailManagerSingleton();
	};
	
})();
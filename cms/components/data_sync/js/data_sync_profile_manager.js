var DataImportProfileManager = new Class
({
	targetClass: "",
	profile_id:  0,
	dialog: 	 Class.Empty,
	
	initialize: function(target, profile_id)
	{
		this.targetClass = target;
		this.profile_id = profile_id;
		
		if (!this.profile_id) this.createProfileDialog();
		
		$$("#DataImportFieldMapping_form input[type=submit]").each(function(submit)
		{
			submit.addEvent('click', function(event) { return this.updateProfileIDs(); }.bind(this));
		}.bind(this));
	},
	
	createProfileDialog: function()
	{
		this.dialog = modalPopup("Create Import Profile", "/action/data_sync/import_profile_dialog?class=" + this.targetClass, "500", "auto", true);
	},
	
	profileEdited: function(response)
	{
		if (response.search(/^\d+$/) == 0)
		{
			window.location.href = "?import_profile_id=" + response;
		}
		else
		{
			$('DataImportProfile_form__error').set({'text': result, 'display': 'table-cell'});
		}
	},
	
	updateProfileIDs: function()
	{
		$$("#DataImportFieldMapping_form input[type=hidden]").each(function (element)
		{
			if (element.id.endsWith("import_profile_id"))
			{
				element.value = this.profile_id;
			}
		}.bind(this));
		
		return true;
	}
	
});
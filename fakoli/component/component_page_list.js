var ComponentPageList = (function()
{
	var ComponentPageListSingleton = new Class(
	{
		rolePopupDialog: null,
		rolePopupModalDialog: null,
		sitePopupDialog: null,
		templatePopupDialog: null,
		
		initialize: function()
		{
		},
		
		toggleEnabled: function(img, id)
		{
			var enable;
			if (img.alt == "Enabled")
			{
				enable = 0;
			}
			else
			{
				enable = 1;
			}
			
			var request = new Request({
				url: "/action/component/update_page?component_page_id=" + id +"&enabled=" + enable,
				method: 'get',
				onSuccess: function (response) 
				{ 
					if (response == 1) 
					{
						img.src = enable ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
						img.alt = enable ? "Enabled" : "Disabled";
					}
				},
				onFailure: function () {},
				async: true
			});
			request.send();
		},

		setEnabled: function(enable)
		{
			var ids = $('component_pages').getSelectedValues();
			
			var action = this.constructAction(ids, "enabled=" + enable);
			var request = new Request({
				url: action,
				method: 'get',
				onSuccess: function (response) 
				{ 
					if (response == 1) 
					{
						ids.each(function(id) 
						{
							var img = $('enable_' + id);
							img.src = enable ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
							img.alt = enable ? "Enabled" : "Disabled";
						});
					}
				},
				onFailure: function () {},
				async: true
			});
			request.send();
			
		},
		
		constructAction: function(ids, params)
		{
			var action = "/action/component/update_page?";
			
			ids.each(function(id) { action += "component_page_id[]=" + id + "&"; });
			
			action += params;
			
			return action;
		},
		
		changeTemplate: function(template, id)
		{
			var request = new Request({
				url: "/action/component/update_page?component_page_id=" + id +"&template=" + encodeURIComponent(template),
				method: 'get',
				onSuccess: function (response) {},
				onFailure: function () {},
				async: true
			});
			request.send();
		},


		rolePopup: function(roles, id, source)
		{
			var popup = $('rolePopup');
			
			if (!this.rolePopupDialog)
			{
				this.rolePopupDialog = new FloatingDialog('rolePopup', {'closeLink': $('closeRolePopup'), 'position': 'absolute', 'width': 200});
			}

			popup.getElements("input[type='checkbox']").each(function(e) { e.checked = false; });

			roles = $(roles);
			var val = roles.get('text');
			var tokens = val.split(',');

			tokens.each(function(t) { if (t != '') $('role_' + t).checked = true; });

			this.rolesMulti = false;
			
			this.rolePopupDialog.targetID = id;
			this.rolePopupDialog.roles = roles;
			this.rolePopupDialog.top = source.getCoordinates().top;
			this.rolePopupDialog.left = source.getCoordinates().left - 204;
			this.rolePopupDialog.show();			
		},

		rolePopupMulti: function()
		{
			var popup = $('rolePopup');
			
			if (!this.rolePopupModalDialog)
			{
				this.rolePopupModalDialog = new ModalDialog('rolePopup', {'closeLink': $('closeRolePopup'), 'width': 200});
			}

			popup.getElements("input[type='checkbox']").each(function(e) { e.checked = false; });

			this.rolePopupModalDialog.show();
			
			this.rolesMulti = true;
		},
		
		updateRoles: function()
		{
			if (this.rolesMulti)
			{
				this.updateRolesMulti();
				return;
			}
			
			var id = this.rolePopupDialog.targetID;
				
			var popup = $('rolePopup');

			var roles = [];
			
			popup.getElements("input[type='checkbox']").each(function(e) { if (e.checked) roles.push(e.value); });

			var role = roles.join(",");
			
			var request = new Request({
				url: "/action/component/update_page?component_page_id=" + id +"&role=" + encodeURIComponent(role),
				method: 'get',
				onSuccess: function (response) 
				{
					if (response == 1)
					{
						this.rolePopupDialog.roles.set('text', role);
						this.rolePopupDialog.roles = Class.Empty;
						this.rolePopupDialog.targetID = 0;
						this.rolePopupDialog.hide();
					}
				}.bind(this),
				onFailure: function () {},
				async: true
			});
			request.send();
		},
		
		updateRolesMulti: function()
		{
			var ids = $('component_pages').getSelectedValues();
			var popup = $('rolePopup');

			var roles = [];
			
			popup.getElements("input[type='checkbox']").each(function(e) { if (e.checked) roles.push(e.value); });

			var role = roles.join(",");
			
			var action = this.constructAction(ids, "role" + encodeURIComponent(role));
			var request = new Request({
				url: action,
				method: 'get',
				onSuccess: function (response) 
				{ 
					if (response == 1) 
					{
						ids.each(function(id) 
						{
							var span = $('roles_' + id);
							span.set('text', role);
						});
					}
					this.rolePopupModalDialog.hide();
				}.bind(this),
				onFailure: function () {},
				async: true
			});
			request.send();
		},
		
		sitePopup: function(id)
		{
			if (!id || id.length == 0) return;	
			
			if (!this.sitePopupDialog)
			{
				this.sitePopupDialog = new ModalDialog('sitePopup', {'closeLink': $('closeSitePopup')});
			}

			this.sitePopupDialog.targetID = id;
			this.sitePopupDialog.show();			
		},
		
		updateSites: function()
		{
			var popup = $('sitePopup');

			var action = "/action/component/update_page?";
			this.sitePopupDialog.targetID.each(function(id) { action += "component_page_id[]=" + id + "&"; });
			
			action += "site_id=" + $('site_id').value;
			
			var request = new Request({
				url: action,
				method: 'get',
				onSuccess: function (response) 
				{
					if (response == 1)
					{
						this.sitePopupDialog.targetID = [];
						this.sitePopupDialog.hide();
					}
				}.bind(this),
				onFailure: function () {},
				async: true
			});
			request.send();
		},
		
		templatePopup: function(id)
		{
			if (!id || id.length == 0) return;
			
			
			if (!this.templatePopupDialog)
			{
				this.templatePopupDialog = new ModalDialog('templatePopup', {'closeLink': $('closeTemplatePopup')});
			}

			this.templatePopupDialog.targetID = id;
			this.templatePopupDialog.show();			
		},
		
		updateTemplates: function()
		{
			var ids = $('component_pages').getSelectedValues();
			var template = $('template_id').value;
			
			var action = this.constructAction(ids, "template=" + template);
			var request = new Request({
				url: action,
				method: 'get',
				onSuccess: function (response) 
				{ 
					if (response == 1) 
					{
						ids.each(function(id) 
						{
							var select = $('template_' + id);
							select.value = template;
						});
					}
					this.templatePopupDialog.hide();
				}.bind(this),
				onFailure: function () {},
				async: true
			});
			request.send();
		}
	});
	
	var instance;
	return function()
	{
		return instance ? instance : instance = new ComponentPageListSingleton();
	};
	
})();
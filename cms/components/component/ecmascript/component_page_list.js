var ComponentPageList = (function()
{
	class ComponentPageListSingleton
	{
		constructor()
		{
			this.rolePopupDialog = null;
			this.rolePopupModalDialog = null;
			this.sitePopupDialog = null;
			this.templatePopupDialog = null;
		}

		toggleEnabled(img, id)
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

			fetch("/action/component/update_page?component_page_id=" + id + "&enabled=" + enable)
				.then(r => r.text())
				.then((response) =>
				{
					if (response == 1)
					{
						img.src = enable ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
						img.alt = enable ? "Enabled" : "Disabled";
					}
				});
		}

		setEnabled(enable)
		{
			var ids = document.getElementById('component_pages').getSelectedValues();

			var action = this.constructAction(ids, "enabled=" + enable);
			fetch(action)
				.then(r => r.text())
				.then((response) =>
				{
					if (response == 1)
					{
						ids.forEach((id) =>
						{
							var img = document.getElementById('enable_' + id);
							img.src = enable ? "/fakoli/images/on.png" : "/fakoli/images/off.png";
							img.alt = enable ? "Enabled" : "Disabled";
						});
					}
				});
		}

		constructAction(ids, params)
		{
			var action = "/action/component/update_page?";

			ids.forEach((id) => { action += "component_page_id[]=" + id + "&"; });

			action += params;

			return action;
		}

		changeTemplate(template, id)
		{
			fetch("/action/component/update_page?component_page_id=" + id + "&template=" + encodeURIComponent(template))
				.then(r => r.text());
		}

		rolePopup(roles, id, source)
		{
			var popup = document.getElementById('rolePopup');

			if (!this.rolePopupDialog)
			{
				this.rolePopupDialog = new FloatingDialog('rolePopup', {'closeLink': document.getElementById('closeRolePopup'), 'position': 'absolute', 'width': 200});
			}

			popup.querySelectorAll("input[type='checkbox']").forEach((e) => { e.checked = false; });

			roles = document.getElementById(roles);
			var val = roles.textContent;
			var tokens = val.split(',');

			tokens.forEach((t) => { if (t != '') document.getElementById('role_' + t).checked = true; });

			this.rolesMulti = false;

			this.rolePopupDialog.targetID = id;
			this.rolePopupDialog.roles = roles;
			var sourceRect = source.getBoundingClientRect();
			this.rolePopupDialog.top = sourceRect.top + window.scrollY;
			this.rolePopupDialog.left = sourceRect.left + window.scrollX - 204;
			this.rolePopupDialog.show();
		}

		rolePopupMulti()
		{
			var popup = document.getElementById('rolePopup');

			if (!this.rolePopupModalDialog)
			{
				this.rolePopupModalDialog = new ModalDialog('rolePopup', {'closeLink': document.getElementById('closeRolePopup'), 'width': 200});
			}

			popup.querySelectorAll("input[type='checkbox']").forEach((e) => { e.checked = false; });

			this.rolePopupModalDialog.show();

			this.rolesMulti = true;
		}

		updateRoles()
		{
			if (this.rolesMulti)
			{
				this.updateRolesMulti();
				return;
			}

			var id = this.rolePopupDialog.targetID;

			var popup = document.getElementById('rolePopup');

			var roles = [];

			popup.querySelectorAll("input[type='checkbox']").forEach((e) => { if (e.checked) roles.push(e.value); });

			var role = roles.join(",");

			fetch("/action/component/update_page?component_page_id=" + id + "&role=" + encodeURIComponent(role))
				.then(r => r.text())
				.then((response) =>
				{
					if (response == 1)
					{
						this.rolePopupDialog.roles.textContent = role;
						this.rolePopupDialog.roles = null;
						this.rolePopupDialog.targetID = 0;
						this.rolePopupDialog.hide();
					}
				});
		}

		updateRolesMulti()
		{
			var ids = document.getElementById('component_pages').getSelectedValues();
			var popup = document.getElementById('rolePopup');

			var roles = [];

			popup.querySelectorAll("input[type='checkbox']").forEach((e) => { if (e.checked) roles.push(e.value); });

			var role = roles.join(",");

			var action = this.constructAction(ids, "role" + encodeURIComponent(role));
			fetch(action)
				.then(r => r.text())
				.then((response) =>
				{
					if (response == 1)
					{
						ids.forEach((id) =>
						{
							var span = document.getElementById('roles_' + id);
							span.textContent = role;
						});
					}
					this.rolePopupModalDialog.hide();
				});
		}

		sitePopup(id)
		{
			if (!id || id.length == 0) return;

			if (!this.sitePopupDialog)
			{
				this.sitePopupDialog = new ModalDialog('sitePopup', {'closeLink': document.getElementById('closeSitePopup')});
			}

			this.sitePopupDialog.targetID = id;
			this.sitePopupDialog.show();
		}

		updateSites()
		{
			var action = "/action/component/update_page?";
			this.sitePopupDialog.targetID.forEach((id) => { action += "component_page_id[]=" + id + "&"; });

			action += "site_id=" + document.getElementById('site_id').value;

			fetch(action)
				.then(r => r.text())
				.then((response) =>
				{
					if (response == 1)
					{
						this.sitePopupDialog.targetID = [];
						this.sitePopupDialog.hide();
					}
				});
		}

		templatePopup(id)
		{
			if (!id || id.length == 0) return;

			if (!this.templatePopupDialog)
			{
				this.templatePopupDialog = new ModalDialog('templatePopup', {'closeLink': document.getElementById('closeTemplatePopup')});
			}

			this.templatePopupDialog.targetID = id;
			this.templatePopupDialog.show();
		}

		updateTemplates()
		{
			var ids = document.getElementById('component_pages').getSelectedValues();
			var template = document.getElementById('template_id').value;

			var action = this.constructAction(ids, "template=" + template);
			fetch(action)
				.then(r => r.text())
				.then((response) =>
				{
					if (response == 1)
					{
						ids.forEach((id) =>
						{
							var select = document.getElementById('template_' + id);
							select.value = template;
						});
					}
					this.templatePopupDialog.hide();
				});
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new ComponentPageListSingleton();
	};

})();

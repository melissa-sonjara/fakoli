/**
 * Manages bulk property editing for CMS section content items selected via a
 * tree widget, opening modal dialogs for template, role, permissions, and SSL settings.
 */
class SectionContentManager
{
	constructor(tree)
	{
		this.tree  = $el(tree);
		this.popup = null;
	}

	allProperties()
	{
		this.getValues();
		if (this.values.length == 0) return;
		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=all&" + this.getQueryParams(), '650px', 'auto', true);
	}

	setTemplate()
	{
		this.getValues();
		if (this.values.length == 0) return;
		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=template&" + this.getQueryParams(), '650px', 'auto', true);
	}

	setRole()
	{
		this.getValues();
		if (this.values.length == 0) return;
		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=role&" + this.getQueryParams(), '650px', 'auto', true);
	}

	setPermissions()
	{
		this.getValues();
		if (this.values.length == 0) return;
		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=permissions&" + this.getQueryParams(), '650px', 'auto', true);
	}

	setSSL()
	{
		this.getValues();
		if (this.values.length == 0) return;
		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=ssl&" + this.getQueryParams(), '650px', 'auto', true);
	}

	setPageTitle()
	{
		this.getValues();
		if (this.values.length == 0) return;
		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=override_page_title&" + this.getQueryParams(), '650px', 'auto', true);
	}

	setBodyClass()
	{
		this.getValues();
		if (this.values.length == 0) return;
		this.popup = modalPopup("Content Properties", "/action/section/content_properties?scope=body_class&" + this.getQueryParams(), '650px', 'auto', true);
	}

	cancel()
	{
		if (this.popup) this.popup.hide();
	}

	getValues()
	{
		this.values = Tree.selectedValues(this.tree.id);
	}

	getQueryParams()
	{
		return "section_content_id[]=" + this.values.join("&section_content_id[]=");
	}

	propertiesChanged(response)
	{
		if (response == "OK")
		{
			if (this.popup) this.popup.hide();
			Array.from(this.tree.querySelectorAll("input:checked")).forEach(function(input)
			{
				findAncestor(input, "div").className = 'flagged_node_leaf';
			});
		}
		else
		{
			var errEl = document.getElementById("SectionContent_form__error");
			if (errEl)
			{
				errEl.innerHTML = response;
				errEl.style.display = 'table-cell';
			}
		}
	}

	clearProperties()
	{
		this.getValues();
		if (this.values.length == 0) return;

		var self = this;
		fetch("/action/section/clear_content_properties?" + this.getQueryParams())
		.then(function(r) { return r.text(); })
		.then(function(response)
		{
			if (response == "OK")
			{
				Array.from(self.tree.querySelectorAll("input:checked")).forEach(function(input)
				{
					findAncestor(input, "div").className = 'file_node_leaf';
				});
			}
		});
	}

	positionModules()
	{
		this.getValues();
		if (this.values.length == 0) return;
		this.popup = modalPopup("Position Modules", "/action/section/content_modules?" + this.getQueryParams(), '750px', 'auto', true);
	}

	modulesChanged(response)
	{
		if (response == "OK")
		{
			if (this.popup) this.popup.hide();
			Array.from(this.tree.querySelectorAll("input:checked")).forEach(function(input)
			{
				findAncestor(input, "div").className = 'flagged_node_leaf';
			});
		}
		else
		{
			var errEl = document.getElementById("SectionContentModules_form__error");
			if (errEl)
			{
				errEl.innerHTML = response;
				errEl.style.display = 'table-cell';
			}
		}
	}
}

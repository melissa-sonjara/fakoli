/**
 *
 */
class SectionContentPropertiesManager
{
	constructor(id, identifier)
	{
		this.id = id;
		this.ctrl = document.getElementById(id);
		this.table = document.getElementById(id + "_table");
		this.identifier = identifier;
		this.data = JSON.parse(this.ctrl.value);
		this.dialog = null;

		this.bindControls();
	}

	bindControls()
	{
		this.table.querySelectorAll('tr[data-section_id]').forEach(tr =>
		{
			var section_id = tr.getAttribute('data-section_id');
			var cbox = tr.querySelector("input[type='checkbox']");
			var button = tr.querySelector('a.button');

			if (this.data[section_id])
			{
				cbox.checked = true;
			}

			button.addEventListener('click', e =>
			{
				e.stopPropagation();
				e.preventDefault();
				this.showPropertiesDialog(section_id);
				return false;
			});
		});
	}

	updateIdentifier(identifier)
	{
		this.identifier = identifier;
		this.updateValue();
	}

	updateValue()
	{
		var flattened = [];

		Object.keys(this.data).forEach(key =>
		{
			this.data[key].identifier = this.identifier;
			flattened.push(this.data[key]);
		});
		this.ctrl.value = JSON.stringify(flattened);
	}

	showPropertiesDialog(section_id)
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
	}

	propertiesChanged(response)
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
	}

	cancel()
	{
		this.dialog.hide();
		this.dialog = null;
	}
}

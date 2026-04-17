/**
 * Manages an AutoForm instance, providing helpers for showing/hiding fields,
 * partial (AJAX) saving, submit button control, and inline error display.
 */
class AutoFormManager
{
	constructor(form, options)
	{
		this.form = $el(form);
		this.partialSaveButton = null;
		this.options = Object.assign({
			partialSaveContainer: '',
			partialSaveLabel: 'Save'
		}, options || {});

		if (!this.form) return;

		this.form.manager = this;

		if (this.form.onInitialize)
		{
			this.form.onInitialize();
		}

		if (this.options.partialSaveContainer)
		{
			this.addPartialSaveButton();
		}
	}

	hideField(field)
	{
		var clazz = "." + this.form.id + '_' + field + '_field';
		Array.from(this.form.querySelectorAll(clazz)).forEach(function(element)
		{
			element.style.display = 'none';
		});
	}

	showField(field)
	{
		var clazz = "." + this.form.id + '_' + field + '_field';
		Array.from(this.form.querySelectorAll(clazz)).forEach(function(element)
		{
			element.style.display = (element.tagName.toLowerCase() == "tr") ? 'table-row' : 'block';
		});
	}

	getGroupID(groupName)
	{
		return this.form.id + "_" + codify(groupName) + "_group";
	}

	addGroupClass(group, cssClass)
	{
		$el(this.getGroupID(group)).classList.add(cssClass);
	}

	removeGroupClass(group, cssClass)
	{
		$el(this.getGroupID(group)).classList.remove(cssClass);
	}

	setLabel(field, text)
	{
		$el(this.form.id + "_" + field + "_label").textContent = text;
	}

	dirty()
	{
		if (this.partialSaveButton)
		{
			this.partialSaveButton.classList.remove('saved');
			this.partialSaveButton.classList.add('dirty');
		}
	}

	addPartialSaveButton()
	{
		var container = $el(this.options.partialSaveContainer);
		this.partialSaveButton = document.createElement('a');
		this.partialSaveButton.className = 'button partial_save';
		this.partialSaveButton.innerHTML = this.options.partialSaveLabel;
		this.partialSaveButton.addEventListener('click', function() { this.partialSave(); }.bind(this));
		container.appendChild(this.partialSaveButton);

		var self = this;
		Array.from(this.form.querySelectorAll("input,select,textarea")).forEach(function(elt)
		{
			elt.addEventListener('change', function()
			{
				self.partialSaveButton.classList.remove('saved');
				self.partialSaveButton.classList.add('dirty');
			});
		});

		Array.from(this.form.querySelectorAll("input,textarea")).forEach(function(elt)
		{
			elt.addEventListener('keypress', function()
			{
				self.partialSaveButton.classList.remove('saved');
				self.partialSaveButton.classList.add('dirty');
			});
		});
	}

	partialSave()
	{
		var action = this.form.action ? this.form.action : window.location.href;

		if (this.partialSaveButton)
		{
			this.partialSaveButton.classList.remove('error');
			this.partialSaveButton.classList.add('saving');
		}

		var self = this;
		var formData = new FormData(this.form);

		fetch(action, {
			method: 'POST',
			body: formData,
			headers: { 'X-Partial-Save': 'true' }
		})
		.then(function(r) { return r.json(); })
		.then(function(responseJSON)
		{
			if (responseJSON.status == 'success')
			{
				if (self.partialSaveButton)
				{
					self.partialSaveButton.classList.remove('dirty');
					self.partialSaveButton.classList.remove('saving');
					self.partialSaveButton.classList.add('saved');
				}
				var pkfield = self.form.querySelector("input[name='" + responseJSON.primary_key + "']");
				if (pkfield && pkfield.value == '')
				{
					pkfield.value = responseJSON.primary_key_value;
				}
			}
			else
			{
				if (self.partialSaveButton)
				{
					self.partialSaveButton.classList.remove('saving');
					self.partialSaveButton.classList.add('error');
				}
				notification(responseJSON.error);
			}
		})
		.catch(function()
		{
			notification("Error contacting server");
		});
	}

	getSubmitButton()
	{
		return $el(this.form.id + "_submitButton");
	}

	setSubmitLabel(text)
	{
		var submitButton = this.getSubmitButton();
		if (submitButton.tagName == 'A' || submitButton.tagName == "BUTTON")
		{
			submitButton.innerHTML = text;
		}
		else
		{
			submitButton.value = text;
		}
	}

	setSubmitEnabled(enabled)
	{
		this.getSubmitButton().disabled = !enabled;
	}

	submit()
	{
		this.form.submit();
	}

	rawSubmit()
	{
		if (this.form.setLoading) this.form.setLoading(true);
		this.form.submit();
	}

	showError(error)
	{
		var el = $el(this.form.id + "__error");
		el.innerHTML = error;
		el.style.display = 'table-cell';
	}

	clearError()
	{
		var el = $el(this.form.id + "__error");
		el.innerHTML = '';
		el.style.display = 'none';
	}
}

AutoFormManager.getManager = function(id)
{
	var form = $el(id);
	if (form) return form.manager;
	return null;
};

AutoFormManager.toggleGroup = function(group, state)
{
	var el = $el(group);
	if (state)
	{
		el.style.opacity = '0';
		el.classList.remove('collapsed');
		el.classList.add('expanded');
		fadeIn(el);
	}
	else
	{
		el.classList.remove('expanded');
		el.classList.add('collapsed');
	}
	if (typeof ModalDialog !== 'undefined' && ModalDialog.recenterActiveDialog)
	{
		ModalDialog.recenterActiveDialog();
	}
};

AutoFormManager.showErrorActiveDialog = function(error)
{
	var dialog = (typeof ModalDialog !== 'undefined') ? ModalDialog.getActiveDialog() : null;
	if (!dialog) { notification(error); return; }
	var form = dialog.options.body.querySelector("form");
	if (!form) { notification(error); return; }
	var manager = form.manager;
	if (!manager) { notification(error); return; }
	manager.showError(error);
};

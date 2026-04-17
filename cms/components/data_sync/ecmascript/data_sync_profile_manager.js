class DataImportProfileManager
{
	constructor(target, profile_id)
	{
		this.targetClass = "";
		this.profile_id = 0;
		this.dialog = null;

		this.targetClass = target;
		this.profile_id = profile_id;

		if (!this.profile_id) this.createProfileDialog();

		document.querySelectorAll("#DataImportFieldMapping_form input[type=submit]").forEach((submit) =>
		{
			submit.addEventListener('click', (event) => { return this.updateProfileIDs(); });
		});
	}

	createProfileDialog()
	{
		this.dialog = modalPopup("Create Import Profile", "/action/data_sync/import_profile_dialog?class=" + this.targetClass, "500", "auto", true);
	}

	profileEdited(response)
	{
		if (response.search(/^\d+$/) == 0)
		{
			window.location.href = "?import_profile_id=" + response;
		}
		else
		{
			var errorEl = document.getElementById('DataImportProfile_form__error');
			errorEl.textContent = result;
			errorEl.style.display = 'table-cell';
		}
	}

	updateProfileIDs()
	{
		document.querySelectorAll("#DataImportFieldMapping_form input[type=hidden]").forEach((element) =>
		{
			if (element.id.endsWith("import_profile_id"))
			{
				element.value = this.profile_id;
			}
		});

		return true;
	}
}

var TextLookup = (function()
{
	/**
	 * Singleton managing the text lookup admin interface, including opening
	 * the advanced features dialog for a given class name.
	 */
	class TextLookupSingleton
	{
		constructor()
		{
			this.dialog = null;
		}

		closeDialog()
		{
			if (this.dialog) this.dialog.hide();
		}

		showAdvancedFeatures()
		{
			var class_name = document.getElementById('class_name').value;
			this.dialog = modalPopup('Advanced Features', '/action/text_lookup/advanced_features?class_name=' + class_name, '500px', 'auto');
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new TextLookupSingleton();
	};
})();

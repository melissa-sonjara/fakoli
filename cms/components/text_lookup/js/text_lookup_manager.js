
var TextLookupManager = new Class({
	
});

TextLookupManager.dialog = null;

TextLookupManager.editText = function(text_id)
{
	TextLookupManager.dialog = modalPopup("Edit Text", "/action/text_lookup/text_lookup_dialog?text_id=" + text_id, '950px', 'auto', true);
};

TextLookupManager.editTextResponse = function(response)
{
	if (response == "OK")
	{
		var dest = new URI();
		dest.setData({'version': 'draft'}, true);
		
		TextLookupManager.dialog.hide(function() { go(dest); });
	}
	else
	{
		TextLookupManager.dialog.options.body.getElement("form").manager.showError(response);
	}
};
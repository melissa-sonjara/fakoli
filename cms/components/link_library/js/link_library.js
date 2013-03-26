var LinkLibraryManager = new Class(
{

});

LinkLibraryManager.dialog = Class.Empty;

LinkLibraryManager.closeDialog = function()
{
	LinkLibraryManager.dialog.hide(); 
	LinkLibraryManager.dialog = Class.Empty;
};

LinkLibraryManager.addLink = function(link_library_id)
{
	LinkLibraryManager.dialog = modalPopup("Add a Link", "/action/link_library/link_form?link_library_id=" + link_library_id, 600, 'auto', true);
};

LinkLibraryManager.editLink = function(link_id)
{
	LinkLibraryManager.dialog = modalPopup("Add a Link", "/action/link_library/link_form?link_id=" + link_id, 600, 'auto', true);
};

LinkLibraryManager.linkEdited = function(result)
{
	if (result == "OK")
	{
		document.id('library_link_list').reload(function() { LinkLibraryManager.closeDialog(); });
	}
	else
	{
		document.id("LinkRecord__error").set('html', result);
	}
};


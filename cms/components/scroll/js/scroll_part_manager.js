/**
 * 
 */
var ScrollPartManager = new Class({
	
});

ScrollPartManager.dialog = null;

ScrollPartManager.editPart = function(scroll_id, scroll_part_id)
{
	ScrollPartManager.dialog = modalPopup("Edit Scroll Part", "/action/scroll/scroll_part_dialog?scroll_id=" + scroll_id + "&scroll_part_id=" + scroll_part_id, '950px', 'auto', true);
};

ScrollPartManager.editPartResponse = function(response)
{
	if (response == "OK")
	{
		var dest = new URI();
		dest.setData({'version': 'draft'}, true);
		
		ScrollPartManager.dialog.hide(function() { go(dest); });
	}
	else
	{
		ScrollPartManager.dialog.options.body.getElement("form").manager.showError(response);
	}
};
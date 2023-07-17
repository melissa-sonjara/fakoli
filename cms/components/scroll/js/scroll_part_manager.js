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

ScrollPartManager.copyScrollDialog = function(scroll_id)
{
	ScrollPartManager.dialog = modalPopup("Copy Scroll", "/action/scroll/copy_scroll_dialog?scroll_id=" + scroll_id, '700px', 'auto', true);
};

ScrollPartManager::onCopyScroll = function(result)
{
	if (result == "OK")
	{
		location.reload();
	}
	else
	{
	}
};
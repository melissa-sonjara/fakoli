var ToolHint = new Class({});
ToolHint.popup = null;

ToolHint.show = function(code, title)
{
	var url = '/action/tool_hints/show_hint?code=' + code;
	if (!ToolHint.popup)
	{
		ToolHint.popup = modalPopup(title, url, "auto", "auto", true);
		ToolHint.popup.addEvent('hide', function() { ToolHint.popup = null; });
	}
	else
	{
		ToolHint.popup.show(null, url);
	}
};

ToolHint.hide = function(code)
{
	var request = new Request(
	{
		method: 'get', 
		url: '/action/tool_hints/hide_hint?code=' + code, 
		onSuccess: function()
		{ 
			ToolHint.popup.hide();
		}
	});
	request.send();
};

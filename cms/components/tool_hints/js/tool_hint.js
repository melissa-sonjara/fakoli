var ToolHint = new Class({});
ToolHint.popup = null;

ToolHint.show = function(code, title)
{
	ToolHint.popup = modalPopup(title, '/action/tool_hints/show_hint?code=' + code, "auto", "auto", true);
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

class ToolHint
{
}

ToolHint.popup = null;

ToolHint.show = function(code, title, width)
{
	if (!width) width = "auto";

	var url = '/action/tool_hints/show_hint?code=' + code;
	if (!ToolHint.popup)
	{
		ToolHint.popup = modalPopup(title, url, width, "auto", true);
		ToolHint.popup.addEventListener('hide', function() { ToolHint.popup = null; });
	}
	else
	{
		ToolHint.popup.show(null, url);
	}
};

ToolHint.hide = function(code)
{
	fetch('/action/tool_hints/hide_hint?code=' + code)
		.then(function()
		{
			ToolHint.popup.hide();
		});
};

var InlineEditing = {};

InlineEditing.setup = function()
{
	document.body.querySelectorAll(".inline_editor_toolbar").forEach(function(controls)
	{
		var container = controls.parentElement;
		setStyles(controls, { position: 'absolute', display: 'none', opacity: '0' });
		container.addEventListener('mouseenter', function(event) { InlineEditing.display(event, controls); });
		container.addEventListener('mouseleave', function(event) { InlineEditing.hide(event, controls); });
		var shim = document.createElement('div');
		shim.className = 'inline_border_shim';
		container.appendChild(shim);
	});
};

InlineEditing.display = function(event, controls)
{
	if (controls.style.display == 'block') return;

	var container = controls.parentElement;
	setStyles(controls, { display: 'block', opacity: '0' });
	positionRelative(controls, { relativeTo: container, position: 'topLeft', edge: 'topLeft' });
	fadeIn(controls);

	var shim = container.querySelector('.inline_border_shim');
	setStyles(shim, { display: 'block', width: container.offsetWidth + 'px', height: container.offsetHeight + 'px' });
	positionRelative(shim, { relativeTo: container, position: 'topLeft', edge: 'topLeft' });
};

InlineEditing.hide = function(event, controls)
{
	setStyles(controls, { display: 'none', opacity: '0' });
	var shim = controls.parentElement.querySelector('.inline_border_shim');
	if (shim) shim.style.display = 'none';
};

document.addEventListener('DOMContentLoaded', function()
{
	InlineEditing.setup();
});

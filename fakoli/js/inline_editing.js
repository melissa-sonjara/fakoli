var InlineEditing = new Class({
});

InlineEditing.setup = function()
{
	document.body.getElements(".inline_editor_toolbar").each(function(controls)
	{
		var container = controls.getParent();
		controls.setStyles({position: 'absolute', display: 'none', opacity: 0});
		container.addEvent('mouseenter', function(event) {InlineEditing.display(event, controls); });
		container.addEvent('mouseleave', function(event) {InlineEditing.hide(event, controls); });		
		var shim = new Element('div', {'class': 'inline_border_shim'});
		shim.inject(container);
	});
};

InlineEditing.display = function(event, controls)
{ 
	if (controls.getStyle('display') == 'block') return;
	
	var container = controls.getParent();
	controls.setStyles({display: 'block', opacity: 0});
	controls.position({relativeTo: container, position: 'topLeft', edge: 'topLeft'});
	controls.fade('in');
	
	var shim = container.getElement('.inline_border_shim');
	shim.setStyles({display: 'block', width: container.getWidth(), height: container.getHeight()});
	shim.position({relativeTo: container, position: 'topLeft', edge: 'topLeft'});
};

InlineEditing.hide = function(event, controls)
{
	var container = controls.getParent();
	controls.setStyles({display: 'none', opacity: 0});	
	var shim = container.getElement('.inline_border_shim');
	shim.setStyle('display', 'none');
};

window.addEvent('domready', function() 
{
	InlineEditing.setup();
});
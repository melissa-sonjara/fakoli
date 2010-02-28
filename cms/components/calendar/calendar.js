function showEventBubble(id, link)
{
	$(id).show();
	$(id).fade('in');
	var coord = $(link).getCoordinates();
	$(id).setStyles({'top': coord.top - $(id).getCoordinates().height + 4, 'left': coord.left - 60});
}

function hideEventBubble(id)
{
	$(id).fade('out');
}
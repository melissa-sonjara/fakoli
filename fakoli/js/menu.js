window.addEvent('domready', function()
{
	$$("#global > ul > li").each(function (elt)
	{
		elt.addEvents({'mouseover': function() { elt.addClass("sfhover"); },
					   'mouseout': function() { elt.removeClass("sfhover"); } 
					 });
	});
});

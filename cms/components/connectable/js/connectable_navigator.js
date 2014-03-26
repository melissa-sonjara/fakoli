/**
 * 
 */

var ConnectableNavigator = new Class(
{
	countIndicator: Class.Empty,
	container: Class.Empty,
	
	initialize: function(container)
	{
		this.container = document.id(container);
		this.countIndicator = new CountIndicator(container, {showZero: true});
	}
});

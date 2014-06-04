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
		ConnectableNavigator.navigators.push(this);
	},
	
	refresh: function()
	{
		this.container.reload();
	}
});

ConnectableNavigator.dialog = null;
ConnectableNavigator.navigators = [];

ConnectableNavigator.hideDialog = function()
{
	ConnectableNavigator.dialog.hide();
	ConnectableNavigator.dialog = null;
};

ConnectableNavigator.selectConnectables = function(from, to, name)
{
	ConnectableNavigator.dialog = modalPopup("Select Related " + name, "/action/connectable/select_related?from=" + from + "&to=" + to, 800, 'auto', true);
};

ConnectableNavigator.selectResult = function(result)
{
	if (result == "OK")
	{
		ConnectableNavigator.navigators.each(function(navigator) { navigator.refresh(); });
		ConnectableNavigator.hideDialog();
	}
	else
	{
		alert(result);
	}
};
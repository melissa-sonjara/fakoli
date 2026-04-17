/**
 *
 */

class ConnectableNavigator
{
	constructor(container)
	{
		this.countIndicator = null;
		this.container = null;

		this.container = document.getElementById(container);
		this.countIndicator = new CountIndicator(container, {showZero: true});
		ConnectableNavigator.navigators.push(this);
	}

	refresh()
	{
		this.container.reload(() => { this.countIndicator.refresh(); });
	}
}

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
		ConnectableNavigator.navigators.forEach((navigator) => { navigator.refresh(); });
		ConnectableNavigator.hideDialog();
	}
	else
	{
		alert(result);
	}
};

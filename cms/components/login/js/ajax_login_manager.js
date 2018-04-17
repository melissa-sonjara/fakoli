var AjaxLoginManager = new Class({
	
	Implements: [Options, Events],
	form: null,
	error: null,
	
	options:
	{
		onSuccess: function(redirect) { return true; }
	},
	
	initialize: function(form, error, options)
	{
		this.form = document.id(form);
		this.error = document.id(error);
		this.setOptions(options);
		
		this.form.iFrameFormRequest(
		{
			'onRequest': 	function() { return true; }, 
			'onComplete': 	function(response) { this.onLoginResponse(response); }.bind(this),
			'onFailure':	function(error) { alert(error); }
		});
	},
	
	onLoginResponse: function(response)
	{
		var params = response.split('|');
		if (params[0] == "OK")
		{
			if (this.fireEvent('success', params[1]))
			{
				go(params[1]);
			}
		}
		else
		{
			this.error.set('html', params[1]).setStyle('display', 'block');
		}
	}
});
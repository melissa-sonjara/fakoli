var AjaxLoginManager = new Class({
	
	form: null,
	error: null,
	
	initialize: function(form, error)
	{
		this.form = document.id(form);
		this.error = document.id(error);
		
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
			go(params[1]);
		}
		else
		{
			this.error.set('html', params[1]).setStyle('display', 'block');
		}
	}
});
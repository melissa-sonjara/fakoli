/**
 * Handles subscriber preferences.
 */
var BlogSubscriberPreferenceManager =  new Class
({
	
	initialize: function(div_id)
	{		
		var me = this;
		
		var div = $(div_id);
		var radiobuttons = div.getElements('input');
		
		radiobuttons.each(function(radio)
		{
			radio.addEvent('click', function(e) { me.toggleSelected(this); });
		}.bind(this));		
	},
	
	toggleSelected: function(radio)
	{
		var blog_subscriber_id = radio.name.replace('[]', '');
		this.saveBlogSubscriberPreference(blog_subscriber_id, radio.value);
	},

	saveBlogSubscriberPreference: function(blog_subscriber_id, option)
	{
		var request = new Request({'url': '/action/blog/update_subscription_type?blog_subscriber_id=' + blog_subscriber_id + '&subscription_type=' + option, 
			   'method': 	'get',
			   'onSuccess': function(response) 
		{ 
			if (response == "OK") 
			{
				// nothing to do
			}
			else
			{
				alert(response);
			}
		}.bind(this),
			
		'onFailure':	function() { alert("Failed to communicate with server");}});

		request.send();
	}
});
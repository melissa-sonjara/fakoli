var BlogManager = (function()
{
	var BlogManagerSingleton = new Class(
	{
		dialog: null,
		
		initialize: function()
		{
		},
		
		editBlog: function(blog_id)
		{
			this.dialog = modalPopup('Edit Blog Details', '/action/blog/edit?blog_id=' + blog_id, '900px', 'auto', true);
		},
		
		editResult: function(result)
		{
			if (result == "OK")
			{
				this.closeDialog();
				window.location.reload();
			}
			else if (result == "DELETED")
			{
				window.location.href = "/";
			}
			else
			{
				document.id('Blog_form__error').set({'text': result, 'display': 'table-cell'});
			}
		},
		
		closeDialog: function()
		{
			this.dialog.hide();		
		},
		
		showSubscriptionDialog: function(blog_id, blog_title)
		{
			this.dialog = modalPopup("Subscribe to " + blog_title, "/action/blog/subscribe_dialog?blog_id=" + blog_id, 600, 'auto', true);
		},
		
		subscriptionDialog: function(response)
		{
			
		}
		
	});
	
	var instance = null;
	return function()
	{
		return instance ? instance : instance = new BlogManagerSingleton();
	};
	
})();

var BlogManager = (function()
{
	class BlogManagerSingleton
	{
		constructor()
		{
			this.dialog = null;
		}

		editBlog(blog_id)
		{
			this.dialog = modalPopup('Edit Blog Details', '/action/blog/edit?blog_id=' + blog_id, '900px', 'auto', true);
		}

		editResult(result)
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
				var el = document.getElementById('Blog_form__error');
				el.textContent = result;
				el.style['display'] = 'table-cell';
			}
		}

		closeDialog()
		{
			this.dialog.hide();
		}

		showSubscriptionDialog(blog_id, blog_title)
		{
			this.dialog = modalPopup("Subscribe to " + blog_title, "/action/blog/subscribe_dialog?blog_id=" + blog_id, 600, 'auto', true);
		}

		subscriptionDialogResponse(result)
		{
			if (result == "OK")
			{
				this.dialog.hide(function() { notification("Subscription Updated"); });
			}
			else
			{
				var el = document.getElementById('BlogSubscriber_form__error');
				el.textContent = result;
				el.style['display'] = 'table-cell';
			}
		}
	}

	var instance = null;
	return function()
	{
		return instance ? instance : instance = new BlogManagerSingleton();
	};

})();

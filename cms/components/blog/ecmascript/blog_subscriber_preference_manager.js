/**
 * Handles subscriber preferences.
 */
class BlogSubscriberPreferenceManager
{
	constructor(div_id)
	{
		var div = document.getElementById(div_id);
		var radiobuttons = div.querySelectorAll('input');

		radiobuttons.forEach((radio) =>
		{
			radio.addEventListener('click', () => { this.toggleSelected(radio); });
		});
	}

	toggleSelected(radio)
	{
		var blog_subscriber_id = radio.name.replace('[]', '');
		this.saveBlogSubscriberPreference(blog_subscriber_id, radio.value);
	}

	saveBlogSubscriberPreference(blog_subscriber_id, option)
	{
		fetch('/action/blog/update_subscription_type?blog_subscriber_id=' + blog_subscriber_id + '&subscription_type=' + option, { method: 'GET' })
			.then(r => r.text())
			.then((response) =>
			{
				if (response == "OK")
				{
					// nothing to do
				}
				else
				{
					alert(response);
				}
			})
			.catch(() => { alert("Failed to communicate with server"); });
	}
}

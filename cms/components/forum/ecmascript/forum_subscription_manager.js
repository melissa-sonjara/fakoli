class ForumSubscriptionManager
{
	constructor(forum_id, topic_id, forum_subscription_id, dialog_title)
	{
		this.forum_id = forum_id;
		this.topic_id = topic_id;
		this.forum_subscription_id = forum_subscription_id;
		this.dialog_title = dialog_title;
		this.dialog = null;
	}

	showSubscriptionDialog()
	{
		this.dialog = modalPopup(this.dialog_title, '/action/forum/subscription_dialog?forum_id=' + this.forum_id + '&topic_id=' + this.topic_id + '&forum_subscription_id=' + this.forum_subscription_id, '600px', 'auto', true);
	}

	subscriptionDialogResult(response)
	{
		if (response == "OK")
		{
			window.location.reload();
		}
		else
		{
			document.getElementById('ForumSubscription_form__error').innerHTML = response;
			document.getElementById('ForumSubscription_form__error').style['display'] = 'table-cell';
		}
	}

	closeDialog()
	{
		this.dialog.hide();
	}
}

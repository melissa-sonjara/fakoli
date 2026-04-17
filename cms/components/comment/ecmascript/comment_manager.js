class CommentManager
{
	constructor(component_name, comment_dialog)
	{
		this.dialog = null;
		this.component_name = null;	//< the name of the component
		this.comment_dialog = null;
		this.comment_id = null;

		this.component_name = component_name;
		this.comment_dialog = comment_dialog;
	}

	showCommentDialog(comment_id)
	{
		this.comment_id = comment_id;
		this.dialog = modalPopup('Comment', '/action/' + this.component_name + '/' + this.comment_dialog + '?comment_id=' + comment_id, '520px', 'auto', true);
	}

	commentFormResult(response)
	{
		if (response.indexOf("OK") == 0)
		{
			this.closeDialog();
			var responseFields = response.split("|");
			var elt = document.getElementById('comment_' + this.comment_id + '_title');
			if (elt)
			{
				elt.textContent = responseFields[1];
			}
			elt = document.getElementById('comment_' + this.comment_id + '_message');
			if (elt)
			{
				elt.textContent = responseFields[2];
			}
		}
		else if (response == "DELETE")
		{
			this.closeDialog();
			var elt = document.getElementById('comment_' + this.comment_id);
			if (elt)
			{
				elt.style.display = 'none';
			}
		}
		else
		{
			document.getElementById('EditComment_form__error').innerHTML = response;
		}
	}

	closeDialog()
	{
		this.dialog.hide();
	}
}

var Comment = (function()
{
	class CommentSingleton
	{
		constructor()
		{
			this.dialog = null;
		}

		showCommentDialog(comment_id, xref_class, component)
		{
			this.dialog = modalPopup('Comment', '/action/comment/comment_form?comment_id=' + comment_id + '&xref_class=' + xref_class + "&xref_component=" + component, '520px', 'auto', true);
		}

		commentFormResult(response)
		{
			if (response.indexOf("OK") == 0)
			{
				var responseFields = response.split("|");

				this.reloadCommentPanel(responseFields[1]);
			}
			else
			{
				var errorEl = document.getElementById('Comment_form__error');
				errorEl.innerHTML = response;
				errorEl.style.display = 'table-cell';
			}
		}

		editCommentFormResult(response)
		{
			if (response.indexOf("OK") == 0)
			{
				var responseFields = response.split("|");

				this.reloadCommentPanel(responseFields[1]);
			}
			else
			{
				var errorEl = document.getElementById('EditComment_form__error');
				errorEl.innerHTML = response;
				errorEl.style.display = 'table-cell';
			}
		}

		commentPublish(comment_id)
		{
			fetch('/action/comment/publish_comment?comment_id=' + comment_id)
				.then(r => r.text())
				.then((response) =>
				{
					if (response == "OK")
					{
						this.reloadCommentPanel("");
					}
					else
					{
						alert(response);
					}
				});
		}

		reloadCommentPanel(confirmation_message)
		{
			document.getElementById('comment_panel').reload(() =>
			{
				this.closeDialog();
				this.showConfirmationMessage(confirmation_message);
			});
		}

		showConfirmationMessage(confirmation_message)
		{
			if (!confirmation_message) return;
			notification(confirmation_message);
		}

		closeDialog()
		{
			if (this.dialog)
			{
				this.dialog.hide();
				this.dialog = null;
			}
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new CommentSingleton();
	};

})();

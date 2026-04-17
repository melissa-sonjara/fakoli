class AttachmentManager
{
	constructor(field, list, options)
	{
		this.options = Object.assign(
			{
				title:     "Upload Attachment",
				width:     "500px",
				height:    "auto",
				useCamera: false
			},
			options || {}
		);

		this.field = (typeof field === 'string') ? document.getElementById(field) : field;
		this.list  = (typeof list  === 'string') ? document.getElementById(list)  : list;

		this.deleters = this.list.querySelectorAll('.attachment_delete_link');
		this.deleters.forEach(function(deleter)
		{
			deleter.attachmentManager = this;
		}.bind(this));
	}

	addAttachment()
	{
		var url = '/action/attachment/attachment_form?use_camera=' + (this.options.useCamera ? '1' : '0');
		AttachmentManager.activeManager = this;
		AttachmentManager.uploadDialog  = modalPopup(this.options.title, url, this.options.width, this.options.height, true, true);
	}

	deleteAttachment(name, attachmentId)
	{
		if (confirm("Are you sure you want to delete the attachment '" + name + "'?"))
		{
			var url  = '/action/attachment/delete?attachment_id=' + attachmentId;
			var self = this;

			fetch(url, { method: 'GET' })
				.then(function(r) { return r.text(); })
				.then(function(response)
				{
					if (response === 'OK')
					{
						var li = document.getElementById('attachment_' + attachmentId);
						if (li) li.remove();

						if (self.field.value)
						{
							var values = self.field.value.split(',');
							values = values.filter(function(v) { return v != attachmentId; });
							self.field.value = values.join(',');
						}
					}
					else
					{
						notification("Failed to delete attachment '" + name + "'");
					}
				})
				.catch(function()
				{
					notification("Failed to communicate with server.");
				});
		}
	}

	fileUploaded(result)
	{
		if (result && result.success)
		{
			var attachmentId = result.attachment_id;
			var name         = result.name;
			var size         = result.size;
			var icon         = result.icon;

			this.list.innerHTML +=
				"<li data-attachment_id='" + attachmentId + "' id='attachment_" + attachmentId + "' class='attachment'><span><img src='" + icon + "' alt='Icon' style='display: inline-block;vertical-align: middle'/>&nbsp;" +
				"<a href='/action/attachment/download?attachment_id=" + attachmentId + "'>" + name + "</a>&nbsp;(" + size + ")&nbsp;" +
				"<a class='attachment_delete_link' id='delete_attachment_" + attachmentId + "' href='#' onclick='this.attachmentManager.deleteAttachment(\"" + name + "\", " + attachmentId + "); return false' title='Delete this Attachment'>" +
				"<i class='fas fa-times'></i></a></span></li>";

			if (this.field.value) this.field.value += ',';
			this.field.value += attachmentId;

			var deleteLink = document.getElementById('delete_attachment_' + attachmentId);
			if (deleteLink)
			{
				deleteLink.attachmentManager = this;
			}

			AttachmentManager.uploadDialog.hide();
			AttachmentManager.uploadDialog  = null;
			AttachmentManager.activeManager = null;
		}
	}
}

AttachmentManager.uploadDialog  = null;
AttachmentManager.activeManager = null;

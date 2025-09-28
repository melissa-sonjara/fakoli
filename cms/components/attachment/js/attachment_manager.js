var AttachmentManager = new Class({
    Implements: [Options, Events],
    
    field: null,
    list: null,

    options: {
        title: "Upload Attachment",
        width: "500px",
        height: "auto",
        useCamera: false
    },

    initialize: function(field, list, options) {
        this.setOptions(options);

        this.field = document.id(field);
        this.list = document.id(list);

        this.deleters = this.list.getElements('.attachment_delete_link');
        this.deleters.each(function(deleter) {
            deleter.attachmentManager = this;
        }.bind(this));
    },

    addAttachment: function()
    {
        let url = '/action/attachment/attachment_form?use_camera=' + ((this.options.useCamera) ? '1' : '0');
        AttachmentManager.activeManager = this;
        AttachmentManager.uploadDialog = modalPopup(this.options.title, url, this.options.width, this.options.height, true, true);
    },

    deleteAttachment: function(name, attachmentId)
    {
        if (confirm("Are you sure you want to delete the attachment '" + name + "'?"))
        {
            let url = '/action/attachment/delete?attachment_id=' + attachmentId;
            let request = new Request(
            {
                url: url,
                method: 'get',
                onSuccess: function(response)
                {
                    if (response == "OK")
                    {
                        let li = document.id('attachment_' + attachmentId);
                        if (li) li.remove();

                        if (this.field.value)
                        {
                            let values = this.field.value.split(',');
                            values = values.filter(function(value) { return value != attachmentId; });
                            this.field.value = values.join(',');
                        }
                    }
                    else
                    {
                        notification("Failed to delete attachment '" + name + "'");
                    }
                }.bind(this),
                onFailure: function()
                {
                    notification("Failed to communicate with server.");
                }
            });
            request.send();
        }
    },

    fileUploaded: function(result)
    {
        if (result && result.success)
        {
            let attachmentId = result.attachment_id;
            let name = result.name;
            let size = result.size;
            let icon = result.icon;

            this.list.innerHTML += 
                "<li data-attachment_id='" + attachmentId + "' id='attachment_" + attachmentId + "' class='attachment'><span><img src='" + icon + "' alt='Icon' style='display: inline-block;vertical-align: middle'/>&nbsp;" +
                "<a href='/action/attachment/download?attachment_id=" + attachmentId + "'>" + name + "</a>&nbsp;(" + size + ")&nbsp;" +
                "<a class='attachment_delete_link' id='delete_attachment_" + attachmentId + "' href='#' onclick='this.attachmentManager.deleteAttachment(\"" + name + "\", " + attachmentId + "); return false' title='Delete this Attachment'>" +
                "<i class='fas fa-times'></i></a></span></li>";
				
            if (this.field.value) this.field.value += ",";
            this.field.value += attachmentId;
            
            let deleteLink = document.id('delete_attachment_' + attachmentId);
            if (deleteLink) 
            {
                deleteLink.attachmentManager = this;
            }
            AttachmentManager.uploadDialog.hide();
            AttachmentManager.uploadDialog = null;
            AttachmentManager.activeManager = null;
        }
    }
});

AttachmentManager.uploadDialog = null;
AttachmentManager.activeManager = null;

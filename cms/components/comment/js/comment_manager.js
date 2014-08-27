
var CommentManager =  new Class 
({
	dialog: null, 

	component_name: null,	//< the name of the component 	
	comment_dialog: null, 	
	comment_id: null,
	
	initialize: function(
			component_name,
			comment_dialog
			)
	{

		this.component_name = component_name;
		this.comment_dialog = comment_dialog;
	},

	showCommentDialog: function(comment_id)
	{
		this.comment_id = comment_id;
		this.dialog = modalPopup('Comment', '/action/' + this.component_name + '/' + this.comment_dialog + '?comment_id=' + comment_id, '520px', 'auto', true);
	},
		
	commentFormResult: function(response)
	{
		if (response.indexOf("OK") == 0)
		{
			this.closeDialog();
			var responseFields = response.split("|");	
			var elt = document.id('comment_' + this.comment_id + '_title');
			if(elt)
			{
				elt.set("text", responseFields[1]);
			}
			elt = document.id('comment_' + this.comment_id + '_message');
			if(elt)
			{
				elt.set("text", responseFields[2]);
			}
		}
		else if(response == "DELETE")
		{
			this.closeDialog();
			elt = document.id('comment_' + this.comment_id);
			if(elt)
			{
				elt.setStyle('display', 'none');
			}
		}
		else
			document.id('EditComment_form__error').set('html', response);
	},


	
	closeDialog: function()
	{
		this.dialog.hide();
	}
});
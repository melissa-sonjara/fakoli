
var CommentManager =  new Class 
({
	dialog: null, 

	itemPk: null,	//< the id of the table linked to comment through xref 
	item_id: null, //< the id value of the record

	component_name: null,	//< the name of the component 	
	comment_dialog: null, 	
	comment_id: null,
	
	initialize: function(itemPk, item_id, 
			component_name,
			comment_dialog
			)
	{

		this.itemPk = itemPk;
		this.item_id = item_id;
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
			var elt = $('comment_' + this.comment_id + '_title');
			if(elt)
			{
				elt.set("text", responseFields[1]);
			}
			elt = $('comment_' + this.comment_id + '_message');
			if(elt)
			{
				elt.set("text", responseFields[2]);
			}
		}
		else if(response == "DELETE")
		{
			elt = $('comment_' + this.comment_id);
			if(elt)
			{
				elt.setStyle('display', 'none');
			}
		}
		else
			$('EditComment_form__error').set('html', response);
	},


	
	closeDialog: function()
	{
		this.dialog.hide();
	}
});
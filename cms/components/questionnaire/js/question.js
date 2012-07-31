var Question =  (function()
{
	var QuestionSingleton = new Class(
	{
		dialog: null,
		question_count: null,
		form: null,
		form_id: null,
			
		initialize: function()
		{
		},
		
		setup: function(question_type_id, form_id)
		{
			this.form_id = form_id;
			this.form = $(form_id);
			this.setQuestionTypeFields(question_type_id);
		},
		
		setQuestionTypeOptions: function()
		{
			var typeElt = $('question_type_id');
			this.onChangeQuestionType(typeElt);
		},
		
		setQuestionTypeFields: function(type)
		{
			var question_type_id = parseInt(type);
			
			switch(question_type_id)
			{
				case 1: // multiple choice
					this.showOptionsField();
					this.hide_tr(this.form_id + '_num_rows');
					this.hideRatingsFields();
					this.hide_tr(this.form_id + '_char_limit');
					this.renderRequiredCheckbox();
					break;

				case 2: // rating
					this.hideOptionsField();
					this.hide_tr(this.form_id + '_num_rows');
					this.showRatingsFields();
					this.hide_tr(this.form_id + '_char_limit');
					this.renderRequiredCheckbox();
					break;
			
				case 3: // short text
					this.hideOptionsField();
					this.hide_tr(this.form_id + '_num_rows');
					this.hideRatingsFields();
					this.show_tr(this.form_id + '_char_limit');
					this.renderRequiredCheckbox();
					break;
					
				case 4: // free text
					this.hideOptionsField();
					this.show_tr(this.form_id + '_num_rows');
					this.hideRatingsFields();
					this.show_tr(this.form_id + '_char_limit');
					this.renderRequiredCheckbox();
					break;
				
				case 5: // checklist
					this.showOptionsField();
					this.hide_tr(this.form_id + '_num_rows');
					this.hideRatingsFields();
					this.hide_tr(this.form_id + '_char_limit');
					this.renderRequiredNumber();
					break;
					
				case 6: // drop down list
					this.showOptionsField();
					this.hide_tr(this.form_id + '_num_rows');
					this.hideRatingsFields();
					this.hide_tr(this.form_id + '_char_limit');
					this.renderRequiredCheckbox();
					break;
					
				default:
					break;
			}
		},

		onChangeQuestionType: function(typeElt)
		{
			var type = typeElt.value;
		
			var question_type_id = parseInt(type);
			this.setQuestionTypeFields(question_type_id);
		},
		
		hideOptionsField: function()
		{
			var option_label_tr = this.getOptionsLabel();
	
			this.hide_tr(this.form_id + '_options');
			if(option_label_tr)
				option_label_tr.setStyle("display", "none");	
		},
		
		getOptionsLabel: function()
		{
			var option_label_tr;			
			var option_label = this.form.getElements("label[for='options']").each(function(label) 
			{ 
				option_label_tr = findAncestor(label, "tr");
			});
			
			return option_label_tr;
		},
		
		showOptionsField: function()
		{
			var option_label_tr;
			option_label_tr = this.getOptionsLabel();

			this.show_tr(this.form_id + '_options');
			
			if(option_label_tr)
				option_label_tr.setStyle("display", "");
		},
		
		showRatingsFields: function()
		{
			this.show_tr(this.form_id + '_label_for_lowest');
			this.show_tr(this.form_id + '_label_for_highest');
			this.show_tr(this.form_id + '_number_of_steps');
		},
		
		hideRatingsFields: function()
		{
			this.hide_tr(this.form_id + '_label_for_lowest');
			this.hide_tr(this.form_id + '_label_for_highest');
			this.hide_tr(this.form_id + '_number_of_steps');
		},
		
		// Render the required field as a boolean checkbox
		renderRequiredCheckbox: function()
		{
			var requiredElt = $(this.form_id + '_required');		
			var required_label;
			
			var required_label = this.form.getElements("label[for='required']").each(function(label) 
			{ 
				label.set("html", "Answer Required");
			});

			var value;
			value = this.getRequiredValue(requiredElt);
						
			requiredElt.set("type", "checkbox");
			requiredElt.set("size", "");
			requiredElt.set("value", 1);
			if(value > 0)
				requiredElt.checked = true;
			requiredElt.set("onkeypress", "");			
		},
		
		getRequiredValue: function(requiredElt)
		{
			var value;
			
			if(requiredElt.get("type") == "checkbox")
			{
				 if(requiredElt.checked == true)
					 value = 1;
				 else
					 value = 0;
			}
			else
				value = requiredElt.value;
		
			return value;
		},
		
		// render the Required field as a number
		renderRequiredNumber: function()
		{
			var requiredElt = $(this.form_id + '_required');
			
			var required_label;		
			var required_label = this.form.getElements("label[for='required']").each(function(label) 
			{ 
				label.set("html", "Number Required");
			});

			var value;
			value = this.getRequiredValue(requiredElt);
	
			requiredElt.set("type", "text");
			requiredElt.set("size", 5);
			requiredElt.set("value", value);
			requiredElt.set("onkeypress", "return maskInput(event, 0);");
		},

		
		hide_tr: function(id)
		{
			var elt = $(id);
			if(elt)
				var tr = findAncestor(elt, "tr");
			if(tr)
				tr.setStyle("display", "none");		
		},
	
		show_tr: function(id)
		{
			var elt = $(id);
			if(elt)
				var tr = findAncestor(elt, "tr");
			if(tr)
				tr.setStyle("display", "");		
		},
	
		closeDialog: function()
		{
			this.dialog.hide();
		}
		  
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new QuestionSingleton();
	};
	
})();
var QuestionManager =  new Class 
({
	dialog: null,
	question_count: null,
	form: null,
	form_id: null,
	requiredNumber: Class.Empty,
	requiredType: null,			///< The last used required type: checkbox or text
			
	initialize: function(question_type_id, form_id)
	{
		this.form_id = form_id;
		this.form = $(form_id);
		this.createRequired();
		this.setQuestionTypeFields(question_type_id);
		
		me = this;
		$(this.form_id + "_question_type_id").addEvent('change', function(e) {me.onChangeQuestionType(this); });

	},
		
	createRequired: function()
	{
		var elt = $(this.form_id + '_required');
			
		// Save the last used requiredType to know how to retrieved checked or count values
		this.requiredType = elt.get("type");
	
		this.requiredNumber = elt.clone(true, true);
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
				this.multiChoiceQuestion();
				break;

			case 2: // rating
				this.ratingQuestion();
				break;
		
			case 3: // short text
				this.shortTextQuestion();
				break;
				
			case 4: // free text
				this.freeTextQuestion();
				break;
			
			case 5: // checklist
				this.checklistQuestion();
				break;
				
			case 6: // drop down list
				this.dropDownList();
				break;
				
			case 7: // heading only
				this.headingOnly();
				break;
				
			default:
				break;
		}
	},
	
	multiChoiceQuestion: function()
	{
		this.showOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");
	},
	
	ratingQuestion: function()
	{
		this.hideOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.showRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");
	},
	
	shortTextQuestion: function()
	{
		this.hideOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.show_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");
	},
	
	freeTextQuestion: function()
	{
		this.hideOptionsField();
		this.show_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.show_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");	
	},
	
	checklistQuestion: function()
	{
		this.showOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.renderRequired("text");	
	},
	
	dropDownList: function()
	{
		this.showOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");		
	},
	
	headingOnly: function()
	{
		this.hideOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.hideRequired();	
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
		this.form.getElements("label[for='options']").each(function(label) 
		{ 
			option_label_tr = findAncestor(label, "tr");
		}.bind(this));
		
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
		
	/**
	 * Render the required field as a boolean checkbox
	 * 
	 * IE does not allow changing type from checkbox to input
	 * field through javascript. So instead when we want input
	 * rather than checkbox or checkbox rather than input, we create 
	 * a new field and append to the form and drop the old field.
	 * 
	 * @param String type: checkbox or text
	 */
	renderRequired: function(type)
	{
		var elt = $(this.form_id + '_required');	
		var value = 0;
		
		if(this.requiredType == "checkbox")
		{
			if(elt.get("checked")) value = 1;
		}
		else
		{
			value = elt.get("value");
		}
			
		var labelText = "Number Required";
		var new_elt = Class.Empty;
			
		if(type == "checkbox")
		{
			if(value > 1)
			{
				value = 1;
			}
			new_elt = new Element('input', {'id': elt.get("id"), 'class': elt.get("class")});
			new_elt.setAttribute("type", "checkbox");
			new_elt.set("size", "");
			new_elt.setAttribute("name", elt.get("name"));
			new_elt.setAttribute("onkeypress", "");
			labelText = "Answer Required";
		}
		else
		{
			new_elt = this.requiredNumber.clone();
		}
			
		this.form.getElements("label[for='required']").each(function(label) 
		{ 
			label.set("html", labelText);
			new_elt.set("value", value);
		}.bind(this));
			
		// Doesn't work if checkbox set in above block.
		if(type == "checkbox")
		{
			new_elt.set("value", "1");	
			if(value == "1")
			{
				new_elt.setAttribute("checked", "checked");		
			}
		}
		
		new_elt.set("id", elt.get("id"));
		new_elt.setAttribute("name", "required");
	
		new_elt.replaces(elt);
			
		this.requiredType = type;
	},
	
	hide_tr: function(id)
	{
		var elt = $(id);
		if(!elt) return;
	
		var tr = findAncestor(elt, "tr");
			
		if(!tr) return;
		tr.setStyle("display", "none");		
	},
	
	show_tr: function(id)
	{
		var elt = $(id);
		if(!elt) return;
						
		var tr = findAncestor(elt, "tr");
		if(!tr) return;
		tr.setStyle("display", "");		
	},
	
	hideRequired: function()
	{
		var requiredElt = $(this.form_id + '_required');
		requiredElt.setStyle('display', 'none');
	
		this.form.getElements("label[for='required']").each(function(label) 
		{ 
			label.set("html", "");
		});
	},
		
	closeDialog: function()
	{
		this.dialog.hide();
	}
	  
});
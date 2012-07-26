
/*
 * To do: make survey.js use questionFormSetup from this class 
 * instead of code copy. 
 */
var Questionnaire =  (function()
{
	var QuestionnaireSingleton = new Class(
	{
		dialog: null,
		id: null,
			
		initialize: function()
		{
		},
	
	
		setup: function(id, question_type_id)
		{
			this.id = id;
			var form = $(this.id);
			this.setQuestionTypeFields(question_type_id, form);
		},
		
		setQuestionTypeOptions: function()
		{
			var typeElt = $('question_type_id');
			this.onChangeQuestionType(typeElt);
		},
		
		setQuestionTypeFields: function(type, form)
		{
			var question_type_id = parseInt(type);
			
			switch(question_type_id)
			{
				case 1: // multiple choice
					this.showOptionsField(form);
					this.hide_tr(this.id + '_num_rows');
					this.hideRatingsFields();
					this.hide_tr(this.id + '_char_limit');
					this.renderRequiredCheckbox(form);
					break;

				case 2: // rating
					this.hideOptionsField(form);
					this.hide_tr(this.id + '_num_rows');
					this.showRatingsFields();
					this.hide_tr(this.id + '_char_limit');
					this.renderRequiredCheckbox(form);
					break;
			
				case 3: // short text
					this.hideOptionsField(form);
					this.hide_tr(this.id + '_num_rows');
					this.hideRatingsFields();
					this.show_tr(this.id + '_char_limit');
					this.renderRequiredCheckbox(form);
					break;
					
				case 4: // free text
					this.hideOptionsField(form);
					this.show_tr('SurveyQuestion_form_num_rows');
					this.hideRatingsFields();
					this.show_tr('SurveyQuestion_form_char_limit');
					this.renderRequiredCheckbox(form);
					break;
				
				case 5: // checklist
					this.showOptionsField(form);
					this.hide_tr(this.id + '_num_rows');
					this.hideRatingsFields();
					this.hide_tr(this.id + '_char_limit');
					this.renderRequiredNumber(form);
					break;
					
				case 6: // drop down list
					this.showOptionsField(form);
					this.hide_tr(this.id + '_num_rows');
					this.hideRatingsFields();
					this.hide_tr(this.id + '_char_limit');
					this.renderRequiredCheckbox(form);
					break;
					
				default:
					break;
			}
		},
		
		onChangeQuestionType: function(typeElt)
		{
			var type = typeElt.value;
			var form = $(this.id);
		
			var question_type_id = parseInt(type);
			this.setQuestionTypeFields(question_type_id, form);
		},
		
		hideOptionsField: function(form)
		{
			var option_label_tr = this.getOptionsLabel(form);
	
			this.hide_tr(this.id + '_options');
			if(option_label_tr)
				option_label_tr.setStyle("display", "none");	
		},
		
		getOptionsLabel: function(form)
		{
			var option_label_tr;			
			var option_label = form.getElements("label[for='options']").each(function(label) 
			{ 
				option_label_tr = findAncestor(label, "tr");
			});
			
			return option_label_tr;
		},
		
		showOptionsField: function(form)
		{
			var option_label_tr;
			option_label_tr = this.getOptionsLabel(form);

			this.show_tr(this.id + '_options');
			
			if(option_label_tr)
				option_label_tr.setStyle("display", "");
		},
		
		showRatingsFields: function()
		{
			var options = $(this.id + '_options').get("html");
			var ratings = options.split("\n");

			var items = new Array(
					$(this.id + '_label_for_lowest'),
					$(this.id + '_label_for_highest'),
					$(this.id + '_number_of_steps')
				);
			
			var i = 0;
			var count = ratings.length;
			for (i=0; i < count; i++)
			{
				var item = items[i];
				item.set("value", ratings[i]);
				this.show_tr(item);
			}	
		},
		
		hideRatingsFields: function()
		{
			this.hide_tr(this.id + '_label_for_lowest');
			this.hide_tr(this.id + '_label_for_highest');
			this.hide_tr(this.id + '_number_of_steps');
		},
		
		// Render the required field as a boolean checkbox
		renderRequiredCheckbox: function(form)
		{
			var requiredElt = $(this.id + '_required');		
			var required_label;
			
			var required_label = form.getElements("label[for='required']").each(function(label) 
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
		renderRequiredNumber: function(form)
		{
			var requiredElt = $(this.id + '_required');
			
			var required_label;		
			var required_label = form.getElements("label[for='required']").each(function(label) 
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
		
		showQuestionDetails: function(question_id)
		{
			this.dialog = modalPopup('Question', '/action/questionnaire/question_details?question_id=' + question_id, '500px', 'auto', true);		
		},	
		
		showSampleQuestionnaire: function()
		{
			this.dialog = modalPopup('Sample Questionnaire', '/action/questionnaire/sample_questionnaire', '600px', 'auto', true);		
		},	
	
		closeDialog: function()
		{
			this.dialog.hide();
		}
		  
		});

	var instance;
	return function()
	{
		return instance ? instance : instance = new QuestionnaireSingleton();
	};
	
})();
class QuestionManager
{
	constructor(question_type_id, form_id)
	{
		this.dialog = null;
		this.question_count = null;
		this.form = null;
		this.form_id = null;
		this.requiredNumber = null;
		this.requiredType = null;			///< The last used required type: checkbox or text

		this.form_id = form_id;
		this.form = document.getElementById(form_id);
		this.createRequired();
		this.setQuestionTypeFields(question_type_id);

		me = this;
		document.getElementById(this.form_id + "_question_type_id").addEventListener('change', function(e) { me.onChangeQuestionType(this); });
	}

	createRequired()
	{
		var elt = document.getElementById(this.form_id + '_required');

		// Save the last used requiredType to know how to retrieved checked or count values
		this.requiredType = elt.type;

		this.requiredNumber = elt.cloneNode(true);
	}

	setQuestionTypeOptions()
	{
		var typeElt = document.getElementById('question_type_id');
		this.onChangeQuestionType(typeElt);
	}

	setQuestionTypeFields(type)
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
	}

	multiChoiceQuestion()
	{
		this.showOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");
	}

	ratingQuestion()
	{
		this.hideOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.showRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");
	}

	shortTextQuestion()
	{
		this.hideOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.show_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");
	}

	freeTextQuestion()
	{
		this.hideOptionsField();
		this.show_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.show_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");
	}

	checklistQuestion()
	{
		this.showOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.renderRequired("text");
	}

	dropDownList()
	{
		this.showOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.renderRequired("checkbox");
	}

	headingOnly()
	{
		this.hideOptionsField();
		this.hide_tr(this.form_id + '_num_rows');
		this.hideRatingsFields();
		this.hide_tr(this.form_id + '_char_limit');
		this.hideRequired();
	}

	onChangeQuestionType(typeElt)
	{
		var type = typeElt.value;

		var question_type_id = parseInt(type);
		this.setQuestionTypeFields(question_type_id);
	}

	hideOptionsField()
	{
		var option_label_tr = this.getOptionsLabel();

		this.hide_tr(this.form_id + '_options');
		if(option_label_tr)
			option_label_tr.style['display'] = "none";
	}

	getOptionsLabel()
	{
		var option_label_tr;
		this.form.querySelectorAll("label[for='options']").forEach(function(label)
		{
			option_label_tr = findAncestor(label, "tr");
		}.bind(this));

		return option_label_tr;
	}

	showOptionsField()
	{
		var option_label_tr;
		option_label_tr = this.getOptionsLabel();

		this.show_tr(this.form_id + '_options');

		if(option_label_tr)
			option_label_tr.style['display'] = "";
	}

	showRatingsFields()
	{
		this.show_tr(this.form_id + '_label_for_lowest');
		this.show_tr(this.form_id + '_label_for_highest');
		this.show_tr(this.form_id + '_number_of_steps');
	}

	hideRatingsFields()
	{
		this.hide_tr(this.form_id + '_label_for_lowest');
		this.hide_tr(this.form_id + '_label_for_highest');
		this.hide_tr(this.form_id + '_number_of_steps');
	}

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
	renderRequired(type)
	{
		var elt = document.getElementById(this.form_id + '_required');
		var value = 0;

		if(this.requiredType == "checkbox")
		{
			if(elt.checked) value = 1;
		}
		else
		{
			value = elt.value;
		}

		var labelText = "Number Required";
		var new_elt = null;

		if(type == "checkbox")
		{
			if(value > 1)
			{
				value = 1;
			}
			new_elt = document.createElement('input');
			new_elt.id = elt.id;
			new_elt.className = elt.className;
			new_elt.setAttribute("type", "checkbox");
			new_elt.size = "";
			new_elt.setAttribute("name", elt.name);
			new_elt.setAttribute("onkeypress", "");
			labelText = "Answer Required";
		}
		else
		{
			new_elt = this.requiredNumber.cloneNode(true);
		}

		this.form.querySelectorAll("label[for='required']").forEach(function(label)
		{
			label.innerHTML = labelText;
			new_elt.value = value;
		}.bind(this));

		// Doesn't work if checkbox set in above block.
		if(type == "checkbox")
		{
			new_elt.value = "1";
			if(value == "1")
			{
				new_elt.setAttribute("checked", "checked");
			}
		}

		new_elt.id = elt.id;
		new_elt.setAttribute("name", "required");

		elt.parentNode.replaceChild(new_elt, elt);

		this.requiredType = type;
	}

	hide_tr(id)
	{
		var elt = document.getElementById(id);
		if(!elt) return;

		var tr = findAncestor(elt, "tr");

		if(!tr) return;
		tr.style['display'] = "none";
	}

	show_tr(id)
	{
		var elt = document.getElementById(id);
		if(!elt) return;

		var tr = findAncestor(elt, "tr");
		if(!tr) return;
		tr.style['display'] = "";
	}

	hideRequired()
	{
		var requiredElt = document.getElementById(this.form_id + '_required');
		requiredElt.style['display'] = 'none';

		this.form.querySelectorAll("label[for='required']").forEach(function(label)
		{
			label.innerHTML = "";
		});
	}

	closeDialog()
	{
		this.dialog.hide();
	}
}

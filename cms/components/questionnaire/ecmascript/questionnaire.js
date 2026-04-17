var Questionnaire = (function()
{
	class QuestionnaireSingleton
	{
		constructor()
		{
			this.dialog = null;
		}

		showSampleQuestionnaire()
		{
			this.dialog = modalPopup('Sample Questionnaire', '/action/questionnaire/sample_questionnaire', '600px', 'auto', true);
		}

		closeDialog()
		{
			this.dialog.hide();
		}
	}

	var instance;
	return function()
	{
		return instance ? instance : instance = new QuestionnaireSingleton();
	};

})();

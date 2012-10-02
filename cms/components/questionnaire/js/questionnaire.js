
var Questionnaire =  (function()
{
	var QuestionnaireSingleton = new Class(
	{
		dialog: null,
			
		initialize: function()
		{
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
var TermsAndConditions = new Class({
});

TermsAndConditions.formId = Class.Empty;
TermsAndConditions.id = Class.Empty;
TermsAndConditions.title = "Terms And Conditions";

TermsAndConditions.showTerms = function()
{
	if (TermsAndConditions.formId == Class.Empty) return true;
	if (TermsAndConditions.id == Class.Empty) return true;
	if (TermsAndConditions.dialog != Class.Empty) return true;
	
	var acceptedTerms = document.id(TermsAndConditions.id);
	if (acceptedTerms.value != "0") return true;
	
	TermsAndConditions.Dialog = modalPopup(TermsAndConditions.title, "/action/terms_and_conditions/show", 800, 500, true, false);

	return false;
};

TermsAndConditions.approve = function()
{
	var acceptedTerms = document.id(TermsAndConditions.id);
	acceptedTerms.value = 1;
	
	var form = document.id(TermsAndConditions.formId);
	form.submit();
};
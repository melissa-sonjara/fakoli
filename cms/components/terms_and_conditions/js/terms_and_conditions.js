var TermsAndConditions = new Class({
});

TermsAndConditions.id = Class.Empty;
TermsAndConditions.title = "Terms And Conditions";

TermsAndConditions.showTerms = function()
{
	if (TermsAndConditions.id == Class.Empty) return true;
	if (TermsAndConditions.dialog != Class.Empty) return true;
	
	var acceptedTerms = document.id(TermsAndConditions.id);
	if (acceptedTerms.value != "0") return true;
	
	TermsAndConditions.Dialog = modalPopup(TermsAndConditions.title, "/action/terms_and_conditions/show", 800, 500, true, false);

	return false;
};

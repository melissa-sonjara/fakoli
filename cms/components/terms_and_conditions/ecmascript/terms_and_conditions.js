class TermsAndConditions
{
}

TermsAndConditions.formId = null;
TermsAndConditions.id = null;
TermsAndConditions.title = "Terms And Conditions";

TermsAndConditions.showTerms = function()
{
	if (TermsAndConditions.formId == null) return true;
	if (TermsAndConditions.id == null) return true;
	if (TermsAndConditions.dialog != null) return true;

	var acceptedTerms = document.getElementById(TermsAndConditions.id);
	if (acceptedTerms.value != "0" &&
		acceptedTerms.value != "") return true;

	TermsAndConditions.Dialog = modalPopup(TermsAndConditions.title, "/action/terms_and_conditions/show", 800, 550, true, false);

	return false;
};

TermsAndConditions.approve = function()
{
	var acceptedTerms = document.getElementById(TermsAndConditions.id);
	acceptedTerms.value = 1;

	var form = document.getElementById(TermsAndConditions.formId);
	form.submit();
};

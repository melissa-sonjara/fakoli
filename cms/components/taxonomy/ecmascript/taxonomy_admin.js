class TaxonomyAdmin
{
	constructor(panel, taxonomy_id)
	{
		this.panel = document.getElementById(panel);
		this.taxonomy_id = taxonomy_id;
		this.dialog = null;
	}

	openTermDialog(term_id)
	{
		if (typeof term_id == "undefined") term_id = "";

		this.dialog = modalPopup((term_id) ? "Term Definition" : "Add a Term", "/action/taxonomy/term_dialog?taxonomy_id=" + this.taxonomy_id + "&term_id=" + term_id, '800px', 'auto', true);
	}

	onSave(result)
	{
		this.panel.reload(this.closeTermDialog.bind(this));
	}

	importTermsDialog()
	{
		this.dialog = modalPopup("Import Taxonomy Terms", "/action/taxonomy/taxonomy_terms_import?taxonomy_id=" + this.taxonomy_id, '600px', 'auto', true);
	}

	closeTermDialog()
	{
		this.dialog.hide();
	}

	addTerm()
	{
	}

	removeTerm(term_id)
	{
	}
}

TaxonomyAdmin.cloneTaxonomyDialog = function(taxonomy_id)
{
	TaxonomyAdmin.cloneDialog = modalPopup("Clone this Taxonomy", "/action/taxonomy/clone_taxonomy?taxonomy_id=" + taxonomy_id, '600px', 'auto', true);
};

TaxonomyAdmin.cloneTaxonomyResponse = function(response)
{
	if (response.match(/^\d+$/))
	{
		go('?taxonomy_id=' + response);
	}
};

TaxonomyAdmin.closeCloneDialog = function()
{
	TaxonomyAdmin.cloneDialog.hide();
};

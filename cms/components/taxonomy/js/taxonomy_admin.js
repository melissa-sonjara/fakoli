var TaxonomyAdmin = new Class
({
	panel: Class.Empty,
	dialog: Class.Empty,
	taxonomy_id: 0,
	
	initialize: function(panel, taxonomy_id)
	{
		this.panel = document.id(panel);
		this.taxonomy_id = taxonomy_id;
	},

	openTermDialog: function(term_id)
	{
		if (typeof term_id == "undefined") term_id = "";
		
		this.dialog = modalPopup((term_id) ? "Term Definition" : "Add a Term", "/action/taxonomy/term_dialog?taxonomy_id=" + this.taxonomy_id + "&term_id=" + term_id, '600px', 'auto', true);		
	},
	
	onSave: function(result)
	{
		this.panel.reload(this.closeTermDialog.bind(this));		
	},
	
	importTermsDialog: function()
	{
		this.dialog = modalPopup("Import Taxonomy Terms", "/action/taxonomy/taxonomy_terms_import?taxonomy_id=" + this.taxonomy_id, '600px', 'auto', true);		
	},
	
	closeTermDialog: function()
	{
		this.dialog.hide();
	},
	
	addTerm: function()
	{
	},
	
	removeTerm: function(term_id)
	{
	}
	
});
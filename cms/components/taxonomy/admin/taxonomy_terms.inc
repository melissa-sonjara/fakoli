<?php

Fakoli::using("taxonomy");
Fakoli::usingFeature("auto_form", "data_view");

$menu_item = "Taxonomies";

$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);

$taxonomy = new Taxonomy();

if (!$taxonomy_id)
{
	redirect("taxonomies");
}

$panel = new Panel("taxonomy_terms", "/action/taxonomy/taxonomy_terms_list?taxonomy_id=$taxonomy_id");

$tabs = TaxonomyManager::getTabs($taxonomy_id);

$tabs->writeHTML();

Fakoli::includeScript("/components/taxonomy/js/taxonomy_admin.js");
$script .= <<<ENDSCRIPT
<script type='text/javascript'>
var taxonomyAdmin;

window.addEvent('domready', function() 
{
	taxonomyAdmin = new TaxonomyAdmin('taxonomy_terms', $taxonomy_id);
});
</script>
ENDSCRIPT;

?>
<div id="tab_border">
<?
echo $panel->draw();
?>
<br/>
<a class='button' href='#' onclick='taxonomyAdmin.openTermDialog(); return false'>Add a Taxonomy Term</a>&nbsp;&nbsp;<a class='button' href='#' onclick='taxonomyAdmin.importTermsDialog(); return false;'>Import Taxonomy Terms</a>
<br/>
<br/>
</div>
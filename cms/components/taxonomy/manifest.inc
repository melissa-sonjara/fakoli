<?
class TaxonomyManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "taxonomy";
		$component->description = "Taxonomy management";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array
		(
			"Classification"	=> array(

				"Taxonomies"		=>	array("page"	=>	"/admin/taxonomies",
											  "role"	=>	"admin",
											  "weight"	=>	3))
		);		
	}

	static function getScripts()
	{
		return array("/components/taxonomy/js/taxonomy_facet_handler.js");
	}
	
	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"	=>	array(TaxonomyManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(TaxonomyManager, setDefaults),
					 "RegisterSerializationHandler"	=> array(TaxonomyManager, registerSerializationHandler)
		);
	}
}

?>
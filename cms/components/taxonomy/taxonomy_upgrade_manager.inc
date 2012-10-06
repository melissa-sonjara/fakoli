<?php
/**
 * Manage schema updates for the taxonomy component.
 *
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *
 * @author Andy Green
 */


Fakoli::using("taxonomy", "component");

class TaxonomyUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
		"1.0"	=>	"sql:Initial schema creation",
		"1.1"	=>	"sql:Upgrade taxonomy table to support being an enumerable, publishable feature",
		"1.2"	=>	"sql:Updated taxonomy_term table to link it to taxonomy (doh!)",
		"1.3"	=>	"sql:Added taxonomy_id foreign key to taxonomy_term_association table",
		"1.4"	=>	"sql:Change column name from `primary_key` to `id` in taxonomy_term_association table to avoid naming conflict",
		"1.5"	=>	"sql:Change description to definition in taxonomy_term"
	);

	function TaxonomyUpgradeManager()
	{
		$this->component_name = "taxonomy";

		// sets the sqlFile name according to default
		parent::UpgradeManager();
	}
}

?>
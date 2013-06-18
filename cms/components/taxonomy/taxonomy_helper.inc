<?php
class TaxonomyHelper
{
	/**
	 * Given a taxonomy identifier and an obj, return the
	 * associated terms.
	 * @param String $taxonomy_identifier
	 * @param obj $obj DataItem object associated with the taxonomy
	 * @return string
	 */
	static function formatAssociatedTerms($taxonomy_identifier, $obj, $separator = ", ")
	{
		$pk = $obj->getPrimaryKey();
		
		$terms = Query::create(TaxonomyTerm, "WHERE term_id IN (SELECT term_id FROM taxonomy_term_association WHERE class=:class AND id=:obj_id AND taxonomy_id IN (SELECT taxonomy_id FROM taxonomy WHERE identifier=:identifier)) AND taxonomy_id IN (SELECT taxonomy_id FROM taxonomy WHERE identifier=:identifier) ORDER BY term")
			->bind(":class", get_class($obj), ":obj_id", $obj->get($pk), ":identifier", $taxonomy_identifier)
			->execute();
	
		return formatItems($terms, "{term}", $separator);
	}
}
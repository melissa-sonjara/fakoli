<?php
/**\addtogroup taxonomy
 * @{
 */


/**************************************************************

Copyright (c) 2014 Sonjara, Inc

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

Except as contained in this notice, the name(s) of the above
copyright holders shall not be used in advertising or otherwise
to promote the sale, use or other dealings in this Software
without prior written authorization.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

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
	
	/**
	 * Decorates the target DataItem with all the associated taxonomy terms
	 * @param DataItem $item the target DataItem
	 * @param string $separator the separator to use when formatting multiple taxonomy terms
	 */
	static function decorate($item, $separator = ", ")
	{
		$taxonomies = TaxonomyManager::getAssociatedTaxonomies(get_class($item));
		
		foreach($taxonomies as $taxonomy)
		{
			$item->decorate($taxonomy->identifier, TaxonomyHelper::formatAssociatedTerms($taxonomy->identifier, $item, $separator));
		}
	}
}
/**@}*/?>
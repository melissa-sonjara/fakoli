<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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

class Taxonomy extends DataItem
{
	var $table = "taxonomy";
	var $primary_key = "taxonomy_id";
	var $cacheLocal = true;
	
	var $_termCache = array();
	
	var $fields = array("taxonomy_id"			=>	Number,
						"identifier"			=>	String,
						"taxonomy_name"			=>	String,
						"description"			=>	HTML,
						"associated_classes"	=>	String,
						"published"				=>	Boolean,
						"enable_facet_filter"	=>	Boolean);
	
	var $relations = array("Terms"			=>	TaxonomyTerm);
	
	static function fromIdentifier($identifier)
	{
		return Query::create(Taxonomy, "WHERE identifier=:i")
					->bind(":i", $identifier)
					->executeSingle();
	}
		
	function Associations($constraint = "")
	{
		return $this->getRelatedList(TaxonomyAssociation, "", $constraint);
	}
	
	function Terms($constraint = "")
	{
		return $this->getRelatedList(TaxonomyTerm, "", $constraint);
	}
	
	function countTerms()
	{
		return Query::create(TaxonomyTerm, "WHERE taxonomy_id=:taxonomy")
				    ->bind(":taxonomy", $this->taxonomy_id)
				    ->executeValue("COUNT(1)");
	}
	
	function getAssociatedItems($class)
	{
		$item = new $class;
		$pk = $item->getPrimaryKey();
		
		return Query::create($class, "WHERE $pk in (SELECT id FROM taxonomy_term_association WHERE taxonomy_id=:t AND class=:c)")
					->bind(":t", $this->taxonomy_id,
						   ":c", $class)
					->execute();	
	}
	
	function getAssociatedTerms($item)
	{
		return Query::create(TaxonomyTerm, "WHERE term_id IN (SELECT term_id FROM taxonomy_term_association where taxonomy_id=:t and class=:c and id=:i)")
					 ->bind(":t", $this->taxonomy_id,
					 		":c", get_class($item),
					 		":i", $item->get($item->getPrimaryKey()))
					 ->execute();
	}
	
	function getAssociatedTermIds($item)
	{
		$cl = get_class($item);
		
		if (!isset($this->_termCache[$cl]))
		{
			$this->_termCache[$cl] = IndexedQuery::create(TaxonomyTermAssociationSummary, "WHERE taxonomy_id=:t and class=:c", "id")
												 ->bind(":t", $this->taxonomy_id,
												 		":c", get_class($item))
												 ->execute();
		}
		
		return $this->_termCache[$cl][$item->get($item->getPrimaryKey())]->term_ids;
	}
	
	function getAssociatedTermsByClass($class)
	{
		return Query::create(TaxonomyTerm, "WHERE term_id in (SELECT term_id FROM taxonomy_term_association where taxonomy_id=:id AND class=:cl) ORDER BY term")
					->bind(":id", $this->taxonomy_id, ":cl", $class)
					->execute();
	}
}

?>
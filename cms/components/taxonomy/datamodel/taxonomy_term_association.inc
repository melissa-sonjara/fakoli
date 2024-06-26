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

class TaxonomyTermAssociation extends DataItem
{
	var $table = "taxonomy_term_association";
	var $primary_key = "term_association_id";
	
	var $fields = array("term_association_id"	=>	Number,
						"taxonomy_id"			=>	Number,
						"term_id"				=>	Number,
						"class"					=>	String,
						"id"					=>	Number);
	
	var $relations = array("Term"	=>	TaxonomyTerm);
	
	function Term()
	{
		return $this->getRelated(TaxonomyTerm);
	}
	
	function getAssociatedItem()
	{
		$instance = new $this->class;
		$instance->load($this->primary_key);
		return $instance;
	}
}

class TaxonomyTermAssociationSummary extends DataItem
{
	var $table = "taxonomy_term_association_summary";
	
	var $fields = array(
			"taxonomy_id"			=>	Number,
			"term_ids"				=>	String,
			"class"					=>	String,
			"id"					=>	Number);
	
	var $relations = array("Terms"	=>	TaxonomyTerm);

			
	function Terms()
	{
		if (!$this->term_ids) return array();
		
		return Query::create(TaxonomyTerm, "WHERE term_id in ({$this->term_ids})")->execute();
	}
	
	function getAssociatedItem()
	{
		$instance = new $this->class;
		$instance->load($this->primary_key);
		return $instance;
	}
}
/**@}*/?>
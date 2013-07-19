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

class TaxonomyTerm extends DataItem
{
	var $table = "taxonomy_term";
	var $primary_key = "term_id";
	var $cacheLocal = true;
	
	var $fields = array("term_id"		=>	Number,
						"taxonomy_id"	=>	Number,
						"term"			=>	String,
						"definition"	=>	Text);
	
	var $relations = array("Taxonomy"	=>	Taxonomy);
	
	function Taxonomy()
	{
		return $this->getRelated(Taxonomy);
	}
	
	function getAssociatedItems($class, $constraint = "")
	{
		$constraint = preg_replace("/^\\s*WHERE/i", "AND", $constraint);
		$obj = new $class;
		$pk = $obj->getPrimaryKey();
		
		$items = Query::create($class, "WHERE $pk IN (SELECT id FROM taxonomy_term_association WHERE term_id=:term_id AND class=:cl) $constraint")
					  ->bind(":term_id", $this->term_id,
					  		 ":cl", $class)
					  ->execute();
					  			 
		return $items;
	}
}

?>
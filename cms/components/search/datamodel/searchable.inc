<?php
/**************************************************************

 Copyright (c) 2011 Sonjara, Inc

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

/**
 * The Searchable interface must be implemented by any DataItem classes that 
 * want to be searchable via the unified site search.
 * @author andy
 */
interface Searchable
{
	/**
	 * DataItems must implement this method to provide search functionality
	 * @param mixed $params either search text or a SearchParameters object
	 * @param array $range an array of primary key IDs to which the search must be constrained
	 */
	function search($params, $range = null);
}


abstract class AbstractSearchResult
{
	// Provide access to the properties of the underlying class
	abstract function getPrimaryKey();
	abstract function get($field);
	abstract function prettifyClassName($plural = false);
	
	abstract function relevance();
	abstract function title();
	abstract function date();
	
	/**
	 * Display the item title and any other essential details for
	 * the item such as author and a create date.
	 * 
	 * If Application Settings is set to show text fragment, display
	 * the teaser and a Read More link; otherwise, put the link to the
	 * full item' details directly on the title.
	 */
	abstract function summary();
	
	function formatIcon($icon, $alt)
	{
		return "<img class='search_result_icon' src='$icon' alt='$alt'/>\n";
	}
}

?>
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

class BindingTarget extends DataItem
{
	var $table = "binding_target";
	var $primary_key = "binding_target_id";
	
	var $fields = array("binding_target_id"		=>	'Number',
						"target_class"			=>	'String',
						"associated_classes"	=>	'String',
						"enable_facet_filter"	=>	'Boolean');	
	
	function getBoundItemIds($item)
	{
		$cl = get_class($item);
	
		if (!isset($this->_termCache[$cl]))
		{
			$this->_termCache[$cl] = IndexedQuery::create(BindingSummary, "WHERE target_class=:t and class=:c", "id")
												 ->bind(":t", $this->target_class,
													    ":c", get_class($item))
												 ->execute();
		}
	
		return $this->_termCache[$cl][$item->get($item->getPrimaryKey())]->target_ids;
	}
}

class BindingRecord extends DataItem
{
	var $table = "binding";
	var $primary_key = "binding_id";
	
	var $fields = array("binding_id"	=>	'Number',
						"target_class"	=>	'String',
						"target_id"		=>	'Number',
						"class"			=>	'String',
						"id"			=>	'Number');
}

class BindingSummary extends DataItem
{
	var $table = "binding_summary";
	
	var $fields = array("class" 		=>	'String',
						"id"			=>	'Number',
						"target_class"	=>	'String',
						"target_ids"	=>	'String');
}	
/**@}*/?>

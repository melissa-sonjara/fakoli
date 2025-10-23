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
		"1.5"	=>	"sql:Change description to definition in taxonomy_term",
		"1.6"	=>	"sql:Added taxonomy_term_association_summary view to efficiently query term_id summaries for items",
		"1.7"	=>	"sql:Added enable_facet_filter flag to taxonomy table",
		"1.8"	=>	"sql:Added binding_target and binding tables",
		"1.9"	=>	"sql:Added indexes for binding table",
		"2.0"	=>	"sql:Added binding_summary view",
		"2.1"	=>	"sql:Additional index on taxonomy_term_association to improve performance",
		"2.2"	=>	"sql:Added sort_order to taxonomy_term table",
		"2.3"	=>	"sql:Added sort_order to taxonomy table"
	);

	function __construct()
	{
		$this->component_name = "taxonomy";

		// sets the sqlFile name according to default
		parent::__construct();
	}
}

/**@}*/?>

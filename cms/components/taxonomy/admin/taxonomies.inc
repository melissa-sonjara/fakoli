<?php
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

Fakoli::using("taxonomy");
Fakoli::usingFeature("data_view");

$menu_item = "Taxonomies";

$taxonomies = Query::create(Taxonomy, "ORDER BY taxonomy_name")->execute();

$table = new DataListView($taxonomies, "taxonomies");

$table->column("Taxonomy", "<a href='taxonomy_form?taxonomy_id={taxonomy_id}'>{taxonomy_name}</a>", true)
	  ->column("Identifier", "<a href='taxonomy_form?taxonomy_id={taxonomy_id}'>{identifier}</a>", true)
	  ->column("Associated Classes", "{associated_classes:/,/, /}", true)
	  ->column("# of Terms", "{countTerms()}", true, "text-align: right");
	  
$table->pageSize = 20;
$table->sortable = true;
$table->filter = true;
$table->emptyMessage = "No taxonomies have been created yet.";

$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a class='button' href='taxonomy_form'>Add a New Taxonomy</a>
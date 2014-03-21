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
Fakoli::usingFeature("auto_form", "data_view");

$menu_item = "Taxonomies";

$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);

$taxonomy = new Taxonomy();

if (!$taxonomy_id)
{
	redirect("taxonomies");
}

$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);

if (!$taxonomy_id) Fakoli::end("No taxonomy specified");

$taxonomy = new Taxonomy($taxonomy_id);
$terms = $taxonomy->Terms("ORDER BY sort_order, term");

$table = new DataListView($terms, "terms");
$table->column("Term", "<strong>{term}</a>", true)
	  ->column("Definition", "{definition:200}", true);

$table->enableDragReorder("/action/taxonomy/reorder_terms");
$table->dragText = "<span style='font-size: 10px'>Click and drag to change the display order for terms in this taxonomy</span>";
$table->emptyMessage = "No terms have been added to this taxonomy";
 
$script .= $table->writeScript();

$tabs = TaxonomyManager::getTabs($taxonomy_id);
$tabs->page = "/admin/taxonomy_terms";

$tabs->writeHTML();
?>
<div id="tab_border">
<?
$table->drawView();
?>
<br/>
<a class='button' href='taxonomy_terms?taxonomy_id=<?echo $taxonomy_id?>'>Finished Reordering Taxonomy Terms</a>
<br/>
<br/>
</div>
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
Fakoli::usingFeature("auto_form");

$menu_item = "Taxonomies";

$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);

$taxonomy = new Taxonomy();

if ($taxonomy_id)
{
	$taxonomy->load($taxonomy_id);
}

$form = new AutoForm($taxonomy);
$form->required("identifier", "taxonomy_name");
$form->allowDelete = true;

if ($taxonomy_id)
{
	$form->button("Clone this Taxonomy", "TaxonomyAdmin.cloneTaxonomyDialog({$taxonomy_id}); return false;", null, true);
}

$classes =  TaxonomyManager::getRegisteredClassOptions();

new CheckListFieldRenderer($form, "associated_classes", "Associated Content Items", $classes);

$tabs = TaxonomyManager::getTabs($taxonomy_id);

if ($method == "POST")
{
	if ($form->save())
	{
		if ($form->deleted)
		{
			redirect("/admin/taxonomies");
		}
		else
		{
			$tabs->queryString = "taxonomy_id=".$form->data->taxonomy_id;
			$tabs->next();
		}
	}
}

$script .= <<<ENDSCRIPT
<script type='text/javascript' src='/components/taxonomy/js/taxonomy_admin.js'></script>
ENDSCRIPT;

$script .= $form->writeScript();

$tabs->writeHTML();

?>
<div id="tab_border">
<?
echo $form->drawForm();
?>
</div>
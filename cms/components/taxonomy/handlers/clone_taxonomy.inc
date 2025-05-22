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

Fakoli::assertRole("admin");

$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);

if (!$taxonomy_id)
{
	throw new FakoliException("No taxonomy provided");
}

$taxonomy = new Taxonomy($taxonomy_id);

$clone = new Taxonomy();

$clone->filter = new InclusionFilter("taxonomy_id", "taxonomy_name", "identifier");

$form = new AutoForm($clone, "POST", "/action/taxonomy/clone_taxonomy?taxonomy_id=$taxonomy_id", "CloneTaxonomy_form");
$form->required("taxonomy_name", "identifier");

$form->ajaxSubmit("function(result) {TaxonomyAdmin.cloneTaxonomyResponse(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->button("Cancel", "TaxonomyAdmin.closeCloneDialog()", null, true);
$form->customSaveHandler = array('TaxonomyManager', 'cloneTaxonomy');

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end($clone->taxonomy_id);
	}
}

if (!$clone->taxonomy_name) 
{
	$clone->taxonomy_name = "Copy of {$taxonomy->taxonomy_name}";
	$clone->identifier = "copy_of_{$taxonomy->identifier}";
}

$script .= $form->writeScript();
$form->drawForm();

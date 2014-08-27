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

Fakoli::assertRole("admin");
$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);

if (!$taxonomy_id) Fakoli::end("No taxonomy specified");

$taxonomy = new Taxonomy($taxonomy_id);

$taxonomy->filter = new InclusionFilter("taxonomy_id");
$form = new AutoForm($taxonomy, "POST", "/action/taxonomy/taxonomy_terms_import?taxonomy_id=$taxonomy_id");

$form->ajaxSubmit("function(result) {taxonomyAdmin.onSave(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");

$form->submitLabel = "Import Taxonomy Terms";

$upload = new FileUploadFieldRenderer($form, "terms", "Taxonomy Terms File", uploadTerms);

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}


echo $form->writeScript();
$form->drawForm();

function uploadTerms($field, $taxonomy)
{
	$file = $_FILES[$field]["tmp_name"];
	TaxonomyManager::importTerms($taxonomy, $file);
}

?>
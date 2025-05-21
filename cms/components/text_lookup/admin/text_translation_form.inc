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

/*
 * For admin to translate text that appears on
 * various pages in the site.
 * 
 * Janice Gallant for Sonjara, Inc.
 * 
 * 09/03/2010
 */


Fakoli::using("text_lookup");
Fakoli::usingFeature("auto_form");
Fakoli::usingFile("framework/tab_bar.inc");

$text_translation_id = checkNumeric($_GET["text_translation_id"]);
$text_id = checkNumeric($_GET["text_id"]);

if(!$text_translation_id && !$text_id)
	redirect("/text_lookup_list");
	
$menu_item = "Text Lookup";

$translation = new TextTranslation();

if($text_translation_id)
{
	$translation->load($text_translation_id);
	$text = $translation->TextLookup();
	$text_id = $text->text_id;
}	
else
{
	$translation->text_id = $text_id;
	$text = new TextLookup($text_id);
}		

$title = "Text Translation form for " . $text->code;

$form = new AutoForm($translation);

$redirect = "/admin/text_translations?text_id={$text_id}";
$form->button("Cancel", $redirect);
$form->alias("text", "Translated Text");
$languageSelect = new SelectFieldRenderer($form, "language", "Language", $text->getTranslationOptions());
$textSelect = new RelatedItemSelectFieldRenderer($form, "text_id", "Text", 'TextLookup', "WHERE text_id={$text_id}", "text", "text_id");
$form->readonly("text_id");


$tabs = TextLookupManager::getTabs($text_id);
$tabs->page = "/admin/text_translations";

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">
<?php 
$form->drawForm();
?></div>

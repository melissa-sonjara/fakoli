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
 * Show all text translations for a given text item and
 * allow the user to create another translation or
 * edit an existing one.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 5/24/2011
 */


Fakoli::using("text_lookup");
Fakoli::usingFeature("data_view");
Fakoli::usingFile("framework/tab_bar.inc");

$text_id = checkNumeric($_GET["text_id"]);

if(!$text_id)
	redirect("/text_lookup_list");
	
$menu_item = "Text Lookup";

$text = new TextLookup($text_id);
$title = "Text Translations for " . $text->code;

$texts = Query::create(TextTranslation, "WHERE text_id=:text_id ORDER BY language")
	->bind(":text_id", $text_id)
	->execute();

$table = new DataListView($texts, "textTranslations");
$table->column("Code", "{TextLookup.code}", true)
	->column("Text", "{TextLookup.text}", true)
	->column("Language", "{language}", true)
	->column("Translation", "<a href='/admin/text_translation_form?text_translation_id={text_translation_id}'>{text}</a>", true)
	;
  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "No text translations have been defined.";


$tabs = TextLookupManager::getTabs($text_id);

$script .= $table->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">
<p><label for="text">Text:&nbsp;</label><?php echo $text->text ?></p>
<?php 
$table->drawView();
?>
<br/>
<a class='button' href='/admin/text_translation_form?text_id=<?php echo $text_id ?>'>Add a Translation</a>
<br><br></div>
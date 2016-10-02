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


Fakoli::using("text_lookup");
Fakoli::usingFeature("data_view");

$menu_item = "Text Lookup";

$textLookup = new TextLookup();
$textLookup->filter = new InclusionFilter("text_lookup_id", "category");
$textLookup->fromGET();
$filterForm = new FilterForm($textLookup);
$categoryFilter = new FilterFieldRenderer($filterForm, "category", "Filter By Category");

$constraint = $filterForm->getConstraint();

$texts = Query::create(TextLookup, "{$constraint} ORDER BY code")->execute();

$table = new DataListView($texts, "textLookup");
$table->column("Code", "<a href='/admin/text_lookup_form?text_id={text_id}'>{code}</a>", true)
	->column("Category", "{category}", true)
	->column("Text", textLookupPlainText, true)
	->column("Class Name", "{class_name}", true)
	->column("", "<a href='/admin/text_translations?text_id={text_id}'>Translations</a>", true)
	;
  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "No text lookups have been defined.";

$script .= $table->writeScript();
$script .= $filterForm->writeScript();
$filterForm->drawForm();

$table->drawView();

function textLookupPlainText($textLookup)
{
	return $textLookup->text;
}

?>

<br/>
<a class='button' href='/admin/text_lookup_form'>Add a Text Lookup</a>
<br/>
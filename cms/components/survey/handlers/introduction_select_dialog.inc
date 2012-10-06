<?php
/**************************************************************

Copyright (c) 2007,2008 Sonjara, Inc

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

Fakoli::using("survey", "questionnaire");
Fakoli::usingFeature("data_view", "auto_form");

$surveys = Query::create(Survey, "WHERE introduction != ''")->execute();
$surveys = removeDuplicates($surveys, "introduction");

$table = new DataListView($surveys, "surveys");

$table->column("", "<input  style='border: none' type='radio' class='radio' name='survey_id[]' id='survey_id_{survey_id}' value='{survey_id}' onchange=\"new Survey().introductionSelectResult('{introduction}');\" />", false, "width: 10%")
	->column("Title", "{title}", false, "width: 25%")	
	->column("Introduction", "{introduction}", false, "width: 50%");

$table->emptyMessage = "There are no survey introductions defined.";
$table->sortable = false;
$table->pageSize = 0;
$table->paginate = false;
$table->filter = false;
$table->cssStyle = "width: 90%";


echo $table->writeScript();

$table->drawView();

?>
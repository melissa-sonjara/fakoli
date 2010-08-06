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

Fakoli::using("article");
Fakoli::usingFile("framework/data_view.inc");

$title = "Articles";
$menu_item = "Articles";

$articles = query(Article, "ORDER BY last_modified DESC");

$table = new DataListView($articles, "articles");
$table->column("Title", formatTitle, true)
	  ->column("Article Type", "{article_type}", true)
	  ->column("Author", "{Author.first_name} {Author.last_name}", true)
	  ->column("Last Modified", "{last_modified}", true);
	  
$table->sortable = true;
$table->filter = true;
$table->excelFile = "articles.xls";
$table->pageSize = 20;
$table->emptyMessage = "No articles have been created yet.";

function formatTitle($item)
{
	return "<a title=\"{$item->title}\" href=\"article_form?article_id={$item->article_id}\">". ellipsis($item->title, 80) ."</a>";
}

$table->writeScript();

require_once admin_begin_page;

$table->drawView();
?>
<br/>
<input type="button" class="button" value="  Add an Article  " onclick="go('article_form');">
<?
require_once admin_end_page;
?>
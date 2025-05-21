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
 
require_once "../../include/config.inc";
require_once "../datamodel/news_item.inc";
require_once "../framework/search_form.inc";


require_once "../../include/permissions.inc";

$newsitem = new NewsItem();
//$filter = new ExclusionFilter();
//$filter->add("locked");

//$item->filter = $filter;

$search = new SearchParameters($newsitem);
$search->fromGET();

$title = "Results for News Item Search";

$newsitems = query('NewsItem', $search->generateConstraint());


?>
<!-- <pre><?print_r($search) ?></pre>
<? echo $search->generateConstraint() ?><br/> -->
<table class="list" width="100%">
 <tr>
  <th>Title</th>
  <th>Message</th>
 </tr> 

<?
foreach ($newsitems as $newsitem)
{
?>
 <tr>
  <td><a href="item_form.php?LOC_key=<?echo $newsitem->news_item_id ?>"><?echo $newsitem->title ?></a></td>
  <td><?echo $newsitem->message ?></td>
 </tr>
<?
}

?>
</table>
<?

?>
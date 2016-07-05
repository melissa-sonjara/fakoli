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

Fakoli::using("blog", "login");

$id = checkIdentifier($_GET["blog"]);
$article_id = checkNumeric($_GET["article_id"]);
$year = checkNumeric($_GET["year"]);
$month = checkNumeric($_GET["month"]);
$term_id = checkNumeric($_GET["term_id"]);

if (!$id) $id = "blog";

$blog = Blog::findByIdentifier($id);

if (!$blog)
{
	throw new FakoliException("Unknown Blog");
}

if (!$blog->published)
{
	redirect("/index");
	return;
}

if ($year)
{
	echo "<h3>Archives: ";

	if ($month)
	{
		$month_name = date ("F", mktime(0,0,0,$month,1,0));
		echo $month_name . "&nbsp;";
	}
		echo $year;
		echo "</h3>";
}

if(!$user)
{
	$mgr = new LoginManager();
	$mgr->storeRedirectPage();
}

$view = new BlogView($blog);

if ($article_id)
{
	$view->drawArticle($article_id);
}
else
{
	$view->drawArticleList($year, $month, $term_id);
}

?>
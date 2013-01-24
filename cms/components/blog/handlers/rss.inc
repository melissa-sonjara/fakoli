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

Fakoli::using("blog", "article");

$blog_id = checkNumeric($_GET["blog_id"]);
$identifier = checkIdentifier($_GET["blog"]);

if (!$blog_id && !$identifier)
{
	throw new FakoliException("No blog specified");
}

if ($blog_id)
{
	$blog = new Blog($blog_id);
}
else
{
	$blog = Blog::findByIdentifier($identifier);
}

if (!$blog->published || !checkRole($blog->allow_read))
{
	throw new FakoliException("Access Denied");
}

$url = $blog->getURL();

$constraint = ($blog->max_rss_articles) ? "LIMIT {$blog->max_rss_articles}" : "";

$articles = $blog->Articles();
$timestamp = date("r");

$xmlArticles = formatItems($articles, "<item>\n\t<title>{title:xml}</title>\n\t<description>{message:xml}</description>\n\t<link>{$url}?article_id={article_id}</link>\n\t<guid>{$url}?article_id={article_id}</guid>\n\t<pubDate>{publish_date:r}</pubDate>\n</item>","\n");

header("Content-Type: text/xml");

echo "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
?>
<rss version="2.0">
<channel>
        <title><?echo $blog->title?></title>
        <description><?echo $blog->format("{description:xml}")?></description>
        <link><?echo $url?></link>
        <lastBuildDate><?echo $timestamp?></lastBuildDate>
        <pubDate><?echo $blog->format("{created_date:r}")?></pubDate>
        <ttl>1800</ttl>
<?echo $xmlArticles?>
</channel>
</rss>















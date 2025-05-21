<?php
/**************************************************************

Blog Archives Module

@author Sonja Hubbard for Sonjara, Inc.

Lists every month this year in which there were blog posts, followed by every previous year in which there were blog posts.
Months/years are linked to blog page with month and/or year constraint passed in query string.

SELECT DATE_FORMAT(publish_date,'%M %Y') 
	FROM article where (published=1) and (article_type="Sonjara Blog") and (DATE_FORMAT(publish_date,'%Y') = DATE_FORMAT(curdate(),'%Y'))
		GROUP BY DATE_FORMAT(publish_date,'%M %Y')
		
SELECT DATE_FORMAT(publish_date,'%Y') 
	FROM article where (published=1) and (article_type="Sonjara Blog") and (DATE_FORMAT(publish_date,'%Y') < DATE_FORMAT(curdate(),'%Y'))
		GROUP BY DATE_FORMAT(publish_date,'%Y')

*****************************************************************/

Fakoli::using("blog");
$id = $_GET["blog"];
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

$article_type = $blog->title;

$published_this_year = " AND (DATE_FORMAT(publish_date,'%Y') = DATE_FORMAT(curdate(),'%Y')) GROUP BY publish_date";
$published_last_year = " AND (DATE_FORMAT(publish_date,'%Y') < DATE_FORMAT(curdate(),'%Y')) GROUP BY publish_date";

$articlesThisYear = Query::create('Article', "WHERE published=1 AND publish_date <= curdate() AND article_type='{$article_type}'" . $published_this_year . " ORDER BY publish_date DESC")
				->filter(new InclusionFilter(publish_date))
				->execute();

$articlesBeforeThisYear = Query::create('Article', "WHERE published=1 AND article_type='{$article_type}'" . $published_last_year . " ORDER BY publish_date DESC")
				->filter(new InclusionFilter(publish_date))
				->execute();
				
$blogMonths  = array();
foreach($articlesThisYear as $article)
{
	$date = new DateTime($article->publish_date);
	$blogMonths[] = $date->format('Y-m');
}

$blogYears  = array();
foreach($articlesBeforeThisYear as $article)
{
	$date = new DateTime($article->publish_date);
	$blogYears[] = $date->format('Y');
}

$uniqueMonths = array_unique($blogMonths);
$uniqueYears = array_unique($blogYears);
				
if (count($uniqueMonths) > 0 OR count($uniqueYears) > 0)
{
	if (count($uniqueMonths) > 0)
	{
		echo "<h3>Recent Articles</h3>";
		echo "<ul class='recent_articles'>";
		
		foreach($uniqueMonths as $month)
		{	
			$month_name = date( 'F', mktime(0, 0, 0, substr($month,5,2)) ) . " " . substr($month,0,4);
			echo "<li><a href='" . $blogname . "?year=" . substr($month,0,4) . "&month=" . substr($month,5,2) . "'>$month_name</a></li>";
		}
	}
	
	echo "</ul>";
	
	if (count($uniqueYears) > 0)
	{
		echo "<h3>Archives</h3>";
		echo "<ul class='recent_articles'>";
		
		foreach($uniqueYears as $year)
		{
			echo "<li><a href='" . $blogname . "?year=" . substr($year,0,4) . "'>$year</a></li>";
		}
		
		echo "</ul>";
	}
}

?>

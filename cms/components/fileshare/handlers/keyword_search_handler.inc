<?php
/*
 * Description: Lists keywords in use for the library.
 * 
 * @author: Janice Gallant at Sonjara, Inc.
 * 
 * Date: 3/25/2011
 */

Fakoli::using("fileshare", "user", "document");

$document_library_id = checkNumeric($_GET["document_library_id"]);
$field = mysql_escape_string($_GET["field"]);

if(!$document_library_id)
	ajaxReturn("Keyword Search Handler: no document library id");

/*
 * Given any characters entered into the input box, search for a
 * match on first name, last name, email
 */
	
$search = strtolower($_GET["{$field}"]);

$library = new DocumentLibrary($document_library_id);

$documents = Query::create(Document, "WHERE document_library_id=:l and lower(keywords) like :s")
	->bind(":l", $document_library_id, ":s", "%".$search . "%") 
	->execute();

$keywords = array();

if(count($documents) > 0)
{
	foreach($documents as $document)
	{
		$docKeywords = explode(",", $document->keywords);
		foreach($docKeywords as $dKeyword)
		{
			if(preg_match("/$search/i", $dKeyword) && !in_array($dKeyword, $keywords))
				$keywords[] = $dKeyword;
		}
	}
}

if(count($keywords) > 0)
{
	foreach($keywords as $keyword)
	{			
?>
<div><a href="/fileshare_library_search?document_library_id=<?php echo $document_library_id ?>&keyword=<?php echo $keyword ?>"><b><?echo "{$keyword}" ?></b><br/></a>
</div>
<?php 				
	}
}
?>
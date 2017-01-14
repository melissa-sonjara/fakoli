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
 * Description: Lists keywords in use for the library.
 * 
 * @author: Janice Gallant at Sonjara, Inc.
 * 
 * Date: 3/25/2011
 */

Fakoli::using("fileshare", "user", "document");

$document_library_id = checkNumeric($_GET["document_library_id"]);
$field = mysqli_escape_string($_GET["field"]);

if(!$document_library_id)
	Fakoli::end("Keyword Search Handler: no document library id");

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
<div><a href="/fileshare_library_files?document_library_id=<?php echo $document_library_id ?>&keyword=<?php echo $keyword ?>"><b><?echo "{$keyword}" ?></b><br/></a>
</div>
<?php 				
	}
}
?>
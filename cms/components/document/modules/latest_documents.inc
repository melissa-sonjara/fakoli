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
 * Display the most recent documents added to a given library.
 * 
 */ 
Fakoli::using("document");

$document_library_id = checkNumeric($_GET["document_library_id"]);

if(!$document_library_id)
{
	die("Latest documents module - no document_library_id");
}

$library = new DocumentLibrary($document_library_id);

$documents = $library->Documents("ORDER BY publication_date desc limit 5");

if (count($documents))
{
	echo "<h3 class='document_list'>Latest Documents in {$library->name}</h3>\n";
	echo "<div id='document_list'>\n";
	echo "<ul>\n";
	
	foreach($documents as $document)
	{
		echo "<li>\n";
		echo $document->format("<a href='/action/document/download?document_id={document_id}'>{title}</a>\n");
		echo "</li>\n";
	}
	
	echo "</ul>\n";
	echo "</div>\n";
}

?>
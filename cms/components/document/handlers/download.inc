<?
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

Fakoli::using("document", "user", "fileshare");

$document_id = checkNumeric($_GET["document_id"]);

$document = new Document($document_id);

$library = $document->DocumentLibrary();

if (!checkRole($library->allow_access) && !$library->allowAccess())
{
	if (!$user || !$user->user_id)
	{
		LoginManager::redirectToLogin();
	}
	else
	{
		Fakoli::end("Access Denied");
	}
}

if (!checkRole($document->allow_access))
{
	if (!$user || !$user->user_id)
	{
		LoginManager::redirectToLogin();
	}
	else
	{
		Fakoli::end("Access Denied");
	}
}

$d = new DocumentDownload();
$d->document_id = $document_id;
$d->user_id = ($user) ? $user->user_id : 0;
$d->save();

if ($document->url && !$document->file)
{
	redirect($document->url);
}

//TODO: Folder level security

trace("Download: document filename to download {$document->file}", 3);

$filepath = sanitizePath($library->getLibraryDirectory() . DIRECTORY_SEPARATOR . $document->file);

$mimeType = DocumentHandler::getDocMimeType($document->file);
	
trace("Download: opening document with filepath {$filepath} and mimeType {$mimeType}", 3);

trace("Download: opened document {$document_id}", 3);

$overrideDisposition = Settings::getValue("document", "always_display_PDFs_in_browser");
$disposition = ($overrideDisposition && $mimeType == 'application/pdf') ? "inline" : "attachment";

header("Pragma: private");
header("Content-Disposition: {$disposition};filename=\"" . basename($document->file) . "\"");
header("Cache-Control: must-revalidate");
header("Content-Type: $mimeType");

if (Fakoli::detectXSendFileSupport())
{
	header("X-SendFile: $filepath");
	return;	
}
else
{
	$f = fopen($filepath, "r");

    $size = filesize($filepath);
	header("Content-Length: $size");
			
	fpassthru($f);
	fclose($f);
}

session_write_close();
?>

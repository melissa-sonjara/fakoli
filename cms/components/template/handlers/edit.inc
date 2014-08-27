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

Fakoli::using("template");
Fakoli::assertRole("admin");

$file = sanitizePath($_REQUEST["file"]);

$path = $config['homedir'] . DIRECTORY_SEPARATOR . $file;

if ($method == "POST")
{
	$content = $_POST["content"];
	
	$fp = fopen($path, "w");
	fputs($fp, $content);
	fclose($fp);
	echo "1";
}
else
{
	$content = htmlsafe(file_get_contents($path));
	
	$readonly = !is_writable($path);
		
	?>
	<form method="POST" action="/action/template/edit" id="edit_form">
	<input type="hidden" name="file" value="<?echo htmlsafe($file)?>"/>
	<label>
	File Contents<? if ($readonly) echo "&nbsp;<em>(Read Only)</em>";?>
	</label><br/>
	<textarea id="content" name="content" style="width: 650px; height: 350px; font-size: 10px<? if ($readonly) echo '; read-only: true';?>"><?echo $content?></textarea>
	<br/>
	<br/>
	<input type="submit" value=" Save Changes " class="button"/>	
	</form>
	<script type='text/javascript'>
	document.id('content').resizable();
	document.id('edit_form').iFrameFormRequest({'onComplete': editResult, 'onFailure': function() { alert("Failed to communicate with server"); }});
	</script>
<?
}
?>
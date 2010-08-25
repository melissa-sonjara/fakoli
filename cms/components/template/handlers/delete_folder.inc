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
Fakoli::usingFeature("validation");
Fakoli::assertRole("admin");

$mgr = new TemplateManager($config['homedir'], 'templates');
		
$validator = new ValidationEngine();
$validator->id = "folder_form";
$validator->add(new RequiredValidator("folder_value", "Folder"));

if ($method == "POST")
{
	$errors = $validator->validate();
	if ($errors)
	{
		// Throw the validation error back to the calling AJAX script
		die($errors);
	}
	
	$folder = $_POST["folder_value"];
	
	$path = $config['homedir'] . DIRECTORY_SEPARATOR . $folder;
	
	if (!file_exists($path))
	{
		die("1"); // Already deleted - why worry the user?
	}

	$handle = opendir($path);
	while(($f = readdir($handle)) !== FALSE)
	{
		if ($f != "." && $f != "..") 
		{
			$nonempty = true;
			break;
		}
	}
	
	closedir($handle);
	
	if ($nonempty)
	{
		die("The folder cannot be deleted because it is not empty");
	}
	else
	{
		rmdir($path);
		die("1");
	}	
}
else
{
	$tree = new DirectoryTreeControl("folder");
	$tree->width = 320;
	$tree->height = 150;
	$tree->permissionCallback = array($mgr, excludeVersionControl);
		
	$tree->buildFolderTree("templates", $config['homedir']);

?>
<script type="text/javascript">
<? echo $validator->writeScript(); ?>

$('folder_form').iFrameFormRequest({'onRequest': function() { return validate_folder_form($('folder_form')); }, 'onComplete': deleteFolderResult, 'onFailure': function() { alert("Failed to communicate with server"); }});

</script>
<form name="folder_form" id="folder_form" method="POST" action="/action/template/delete_folder">
<p>Choose the folder you wish to delete using the tree below.<br/>
Please note that only empty folders can be deleted <br/>
(i.e. those which do not contain files or sub-folders).</p>
<table class="form">
 <tr>
  <td style="vertical-align: top"><label>Folder</label></td>
  <td>
	<? $tree->writeHTML(); ?>
  </td>
 </tr>
 <tr>
  <td colspan="2"><input type="submit" class="button" value="Delete Folder"/></td>
 </tr>
</table>
</form>
<?	
}
?>
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
$validator->add(new RequiredValidator("parent_folder_value", "Parent Folder"));
$validator->add(new RequiredValidator("folder_name", "Folder Name"));

if ($method == "POST")
{
	$errors = $validator->validate();
	if ($errors)
	{
		// Throw the validation error back to the calling AJAX script
		die($errors);
	}
	
	$parent_folder = sanitizePath($_POST["parent_folder_value"]);
	$folder_name = sanitizePath($_POST["folder_name"]);
	if (!$parent_folder) $parent_folder = "templates";
	
	$path = $config['homedir'] . DIRECTORY_SEPARATOR . $parent_folder . DIRECTORY_SEPARATOR . $folder_name;
	
	trace($parent_folder, 3);
	trace($folder_name, 3);
	trace($path, 3);
	
	if (file_exists($path))
	{
		die("A file or directory already exists with that name");
	}

	mkdir($path);
	chmod($path, "755");
	die("1");
}
else
{
	$tree = new DirectoryTreeControl("parent_folder");
	$tree->width = 320;
	$tree->height = 150;
		
	$tree->buildFolderTree('templates', $config['homedir']);

?>
<script type="text/javascript">
<? echo $validator->writeScript(); ?>

document.id('folder_form').iFrameFormRequest({'onRequest': function() { return validate_folder_form(document.id('folder_form')); }, 'onComplete': createFolderResult, 'onFailure': function() { alert("Failed to communicate with server"); }});

</script>
<form name="folder_form" id="folder_form" method="POST" action="/action/template/create_folder">
<table class="form">
 <tr>
  <td style="vertical-align: top"><label>Parent Folder</label></td>
  <td>
	<? $tree->writeHTML(); ?>
  </td>
 </tr>
 <tr>
  <td><label>Folder Name</label></td>
  <td><input type="text" name="folder_name" value="" style="width: 320px"/></td>
 </tr>
 <tr>
  <td colspan="2"><input type="submit" class="button" value="Create Folder"/></td>
 </tr>
</table>
</form>
<?	
}
?>
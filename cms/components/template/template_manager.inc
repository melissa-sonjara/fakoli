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

Fakoli::usingFeature("directory_tree");

class TemplateManager
{
	var $base;
	var $folder;
	var $tree;
	
	function TemplateManager($base, $folder)
	{		
		$this->base = $base;
		$this->folder = $folder;		
	}
	
	function excludeVersionControl($folder)
	{
		if (basename($folder) == ".svn") return false;
		return true;
	}
	
	function fileFilter($file)
	{
		return 	endsWith($file, ".css") || 
				endsWith($file, ".tpl") || 
				endsWith($file, ".gif") ||
				endsWith($file, ".png") ||
				endsWith($file, ".jpg");
	}
	
	function writeScript()
	{
		global $dialogs;
		
		ob_start();
?>
<div class="dialog" id="templateManagerDlg">
 <div class="dialog_header" id="templateManagerDlgHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeDocumentManagerDlg' href="javascript:templateManagerDlg.hide();"">Close &times;</a></div>
   <span"style="font-weight: bold" id="templateManagerDlgTitle">Template Manager</span>
  </div>	
 </div>
 <div id="templateManagerDlgBody" class="dialog_body">
 </div>
</div>
<?
		$dialogs .= ob_get_contents();
		ob_end_clean();
		
		ob_start();
?>
<script type="text/javascript">

var templateManagerDlg;

window.addEvent('domready', function()
{
	templateManagerDlg = new ModalDialog($('templateManagerDlg'), {handle: $('templateManagerDlgHeader'), draggable: true, body: $('templateManagerDlgBody')});
});		

function editFile()
{
	file = $('files_value').value;
	$('templateManagerDlgTitle').set('text', file);
	templateManagerDlg.show(function() {}, "/action/template/edit?file=" + encodeURIComponent(file));
}

function editResult(result)
{
	if (result == "1")
	{
		templateManagerDlg.hide();
	}
	else
	{
		//$('Document_form__error').set({'text': result, 'display': 'table-cell'});
	}
}


function uploadFile()
{
	id = $('files_value').value;
	templateManagerDlg.show(function() {}, "/action/template/upload?document_library_id=<?echo $this->library->document_library_id?>");
}


function deleteFile()
{
	if (confirm("Are you sure you want to delete this file?"))
	{
		file = $('files_value').value;
		
		var request = new Request({'url': 		'/action/template/delete?file=' + encodeURIComponent(file), 
								   'method': 	'get',
								   'onSuccess': function(responseText) 
								   { 
								   		if (responseText == "1") 
								   		{
								   			window.location.reload();
								   		}
								   		else
								   		{
								   			alert(responseText);
								   		}
								   },
								   'onFailure':	function() { alert("Failed to communicate with server");}});
		request.send();
	}
}	

function selectionChanged()
{
	var file = $('files_value').value;
	
	if (file != "")
	{
		$('edit').disabled = !file.match(/(\.css|\.tpl)$/);
		$('deleteButton').disabled = false;
	}
	else
	{
		$('edit').disabled = true;
		$('deleteButton').disabled = true;
	}
}

function createFolder()
{
	templateManagerDlg.show(function() {}, "/action/template/create_folder");
}

function createFolderResult(result)
{
	if (result == 1)
	{
		window.location.reload();
	}
	else
	{
		alert(result);
	}
}

function deleteFolder()
{
	templateManagerDlg.show(function() {}, "/action/template/delete_folder");
}

function deleteFolderResult(result)
{
	if (result == 1)
	{
		window.location.reload();
	}
	else
	{
		alert(result);
	}
}

</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		return $script;		
	}
	
	function drawView()
	{
		global $config;		

		$tree = new DirectoryTreeControl("files");
		$tree->width = 500;
		$tree->height = 400;
		$tree->onSelectionChanged = "selectionChanged";
		$tree->onDoubleClick = "editFile";
		$tree->permissionCallback = array($this, excludeVersionControl);
		$tree->icons[".tpl"] = "/fakoli/images/tpl_icon.png";
		$tree->icons[".css"] = "/fakoli/images/css_icon.png";
		
		$tree->selectMode = "file";
		
		$tree->buildFolderTree($this->folder, $this->base);
		$tree->addFiles($this->folder, $this->base, array($this, fileFilter));
				
		$tree->children["templates"]->open = true;
		$tree->writeHTML();
		
?>
		<br/>
		<button id="edit" class="button" disabled="disabled" onclick="editFile()">Edit File...</button>&nbsp;&nbsp;
		<button id="deleteButton" class="button" disabled="disabled" onclick="deleteFile()">Delete File</button>&nbsp;&nbsp;
		<button id="upload" class="button" onclick="uploadFile()">Upload a File...</button>
<?
		if (checkRole($this->library->manage_folders))
		{
?>
		&nbsp;&nbsp;<button id="createFolder" class="button" onclick="createFolder()">Create Folder...</button>&nbsp;&nbsp;
		<button id="deleteFolder" class="button" onclick="deleteFolder()">Delete Folder</button>
<?
		}

	}
	
	/**
	 * Upload a file from the specified form field to the specified folder within the current template base.
	 * @param string $field the form field containing the record for uploaded file
	 * @param string $folder the folder within the template base to which the file is to be uploaded
	 * @return string the path of the uploaded file.
	 */
//	function uploadFile($field, $folder)
//	{
//		
//		if (!$_FILES[$field]) 
//		{
//			die("No upload record found.");
//		}
//		
//		if ($_FILES[$field]["name"]=="") 
//		{
//			die("Upload name is empty");
//		}
//		
//		/* Copy across the uploaded file */
//		$folder = str_replace("/", DIRECTORY_SEPARATOR, $folder);
//		$folder = str_replace("\\", DIRECTORY_SEPARATOR, $folder);
//		
//		$name = $_FILES[$field]["name"];
//		
//		$filepath = $folder . DIRECTORY_SEPARATOR . $name;
//		$base = $this->library->getLibraryDirectory();
//		$dir = $base . DIRECTORY_SEPARATOR . $folder;
//		$fullpath = $dir . DIRECTORY_SEPARATOR . $name;
//		
//		trace("Upload Directory: {$dir}", 3);
//				
//		trace("Uploading file to $fullpath", 3);
//		
//		if (!file_exists($dir))
//		{
//			// If the directory does not exist, create it 
//			mkdir($dir);
//		}
//		
//		if (file_exists($fullpath))
//		{
//			// If a previous copy of the file already exists, remove it
//			unlink($fullpath);
//		}
//		
//		move_uploaded_file($_FILES[$field]["tmp_name"], $fullpath);
//		chmod($fullpath, 0755);
//		
//		return str_replace(DIRECTORY_SEPARATOR, "/", $filepath);
//	}

	/**
	 * Delete the specified document (assuming the user has permission).
	 * @param Document $document the document object to be deleted
	 * @return boolean true if the document was successfully deleted, false otherwise
	 */
//	function deleteDocument($document)
//	{
//		$fullPath = $this->library->getLibraryDirectory() . DIRECTORY_SEPARATOR . $document->file;
//		if (file_exists($fullPath))
//		{
//			unlink($fullPath);
//		}
//		
//		$document->delete();
//		return true;
//	}
}

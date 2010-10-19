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

Fakoli::usingFile("framework/directory_tree.inc");

class DocumentManager
{
	var $library;
	var $docs;
	
	function DocumentManager($library)
	{		
		$this->library = $library;
		if ($this->library->document_library_id) 
		{
			$this->documents = $library->Documents("ORDER BY title");
		}
		else
		{
			$this->documents = array();
		}
	}
	
	function folderPermission($folder)
	{
		global $user;
		
		$permission = true;
		
		if (strpos($folder, "Board Members Only") !== FALSE && !checkRole("board,admin,super"))
		{
			$permission = false;
		}
		
		trace("folderPermission: $folder ($permission)", 3);
		
		return $permission;
	}

	function writeScript()
	{
		global $dialogs;
		
		ob_start();
?>
<div class="dialog" id="documentManagerDlg" style="width: 500px">
 <div class="dialog_header" id="documentManagerDlgHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeDocumentManagerDlg' href="javascript:docManagerDlg.hide();"">Close &times;</a></div>
   <span"style="font-weight: bold" id="documentManagerTitle">Document Manager</span>
  </div>	
 </div>
 <div id="documentManagerDlgBody" class="dialog_body">
 </div>
</div>
<?
		$dialogs .= ob_get_contents();
		ob_end_clean();
		
		ob_start();
?>
<script type="text/javascript">

window.addEvent('domready', function()
{
	docManagerDlg = new ModalDialog($('documentManagerDlg'), {handle: $('documentManagerDlgHeader'), draggable: true, body: $('documentManagerDlgBody')});
});		

function downloadFile()
{
	id = $('files_value').value;
	go("/action/document/download?document_id=" + id);
}

function editFile()
{
	id = $('files_value').value;
	docManagerDlg.show(function() {}, "/action/document/edit?document_id=" + id);
}

function editResult(result)
{
	if (result == "1")
	{
		docManagerDlg.hide();
	}
	else
	{
		$('Document_form__error').set({'text': result, 'display': 'table-cell'});
	}
}


function uploadFile()
{
	id = $('files_value').value;
	docManagerDlg.show(function() {}, "/action/document/upload?document_library_id=<?echo $this->library->document_library_id?>");
}


function deleteFile()
{
	if (confirm("Are you sure you want to delete this file?"))
	{
		id = $('files_value').value;
		
		var request = new Request({'url': 		'/action/document/delete?document_id=' + id, 
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
	var id = $('files_value').value;
	
	if (id > 0)
	{
		$('download').disabled = false;
		$('edit').disabled = false;
		$('deleteButton').disabled = false;
	}
	else
	{
		$('download').disabled = true;
		$('edit').disabled = true;
		$('deleteButton').disabled = true;
	}
}

<?
		if (checkRole($this->library->manage_folders))
		{
?>
function createFolder()
{
	docManagerDlg.show(function() {}, "/action/document/create_folder?document_library_id=<?echo $this->library->document_library_id?>");
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
	docManagerDlg.show(function() {}, "/action/document/delete_folder?document_library_id=<?echo $this->library->document_library_id?>");
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

<?
		}
?>
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
		$tree->width = 700;
		$tree->height = 400;
		$tree->onSelectionChanged = "selectionChanged";
		$tree->onDoubleClick = "downloadFile";
		
		$tree->selectMode = "file";
		$tree->permissionCallback = array($this, folderPermission);
		
		$root = $this->library->getLibraryDirectory();
		
		$tree->buildFolderTree($config["uploaddir"], $root);
				
		foreach($this->documents as $doc)
		{
			$file = str_replace("/", DIRECTORY_SEPARATOR, $doc->file);
			$file = str_replace("\\", DIRECTORY_SEPARATOR, $file);
			
			trace($file, 4);
			$folderNode = $tree->findFolder(dirname($file));
			
			if ($folderNode != null)
			{
				trace("Found Folder {$folderNode->id}", 4);
				$id = str_replace(DIRECTORY_SEPARATOR, "_", $file);
			
				$folderNode->add(new FileNode($id, basename($file), $doc->document_id));
			}
			else
			{
				trace("*** NODE NOT FOUND - $file", 3);
			}
		}
				
		$tree->children["documents"]->open = true;
		
		$page->page_title = "Documents";
		
?>
		 <? $tree->writeHTML(); ?>
		<br/>
		<button id="download" class="button" disabled="disabled"  onclick="downloadFile()"> Download File </button>&nbsp;&nbsp;
		<button id="edit" class="button" disabled="disabled" onclick="editFile()">Edit Details...</button>&nbsp;&nbsp;
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
	 * Upload a file from the specified form field to the specified folder within the current document library.
	 * @param string $field the form field containing the record for uploaded file
	 * @param string $folder the folder within the document library to which the file is to be uploaded
	 * @return string the path of the uploaded file within the document library.
	 */
	function uploadFile($field, $folder)
	{
		
		if (!$_FILES[$field]) 
		{
			die("No upload record found.");
		}
		
		if ($_FILES[$field]["name"]=="") 
		{
			die("Upload name is empty");
		}
		
		/* Copy across the uploaded file */
		$folder = str_replace("/", DIRECTORY_SEPARATOR, $folder);
		$folder = str_replace("\\", DIRECTORY_SEPARATOR, $folder);
		
		$name = $_FILES[$field]["name"];
		
		$filepath = $folder . DIRECTORY_SEPARATOR . $name;
		$base = $this->library->getLibraryDirectory();
		$dir = $base . DIRECTORY_SEPARATOR . $folder;
		$fullpath = $dir . DIRECTORY_SEPARATOR . $name;
		
		trace("Upload Directory: {$dir}", 3);
				
		trace("Uploading file to $fullpath", 3);
		
		if (!file_exists($dir))
		{
			// If the directory does not exist, create it 
			mkdir($dir);
		}
		
		if (file_exists($fullpath))
		{
			// If a previous copy of the file already exists, remove it
			unlink($fullpath);
		}
		
		move_uploaded_file($_FILES[$field]["tmp_name"], $fullpath);
		chmod($fullpath, 0755);
		
		return str_replace(DIRECTORY_SEPARATOR, "/", $filepath);
	}

	/**
	 * Delete the specified document (assuming the user has permission).
	 * @param Document $document the document object to be deleted
	 * @return boolean true if the document was successfully deleted, false otherwise
	 */
	function deleteDocument($document)
	{
		$fullPath = $this->library->getLibraryDirectory() . DIRECTORY_SEPARATOR . $document->file;
		if (file_exists($fullPath))
		{
			unlink($fullPath);
		}
		
		$document->delete();
		return true;
	}
	
	function drawSelectView($selectionChanged = "", $doubleClick = "", $width=700, $height = 400)
	{
		global $config;
		
		$tree = new DirectoryTreeControl("files");
		$tree->width = $width;
		$tree->height = $height;
		$tree->onSelectionChanged = $selectionChanged;
		$tree->onDoubleClick = $doubleClick;
		
		$tree->selectMode = "file";
		$tree->permissionCallback = array($this, folderPermission);
		
		if ($this->documents)
		{
			$root = $this->library->getLibraryDirectory();
			
			$tree->buildFolderTree($config["uploaddir"], $root);
					
			foreach($this->documents as $doc)
			{
				$file = str_replace("/", DIRECTORY_SEPARATOR, $doc->file);
				$file = str_replace("\\", DIRECTORY_SEPARATOR, $file);
				
				trace($file, 4);
				$folderNode = $tree->findFolder(dirname($file));
				
				if ($folderNode != null)
				{
					trace("Found Folder {$folderNode->id}", 4);
					$id = str_replace(DIRECTORY_SEPARATOR, "_", $file);
				
					$folderNode->add(new FileNode($id, basename($file), $doc->document_id));
				}
				else
				{
					trace("*** NODE NOT FOUND - $file", 3);
				}
			}
					
			$tree->children["documents"]->open = true;
			$tree->writeHTML(); 
		}
		else
		{
?>
		<div style="width: <?echo $width?>px;height:<?echo $height?>px;border: solid 1px #000;">&nbsp;</div>
<?			
	
		}
	}
}

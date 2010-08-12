<?php

Fakoli::usingFile("framework/directory_tree.inc");

class DocumentManager
{
	var $library;
	var $docs;
	
	function DocumentManager($library)
	{		
		$this->library = $library;
		$this->documents = $library->Documents("ORDER BY title");
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
	
		$script .= <<<ENDSCRIPT
		
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
			docManagerDlg.show(function() {}, "/action/document/upload?document_library_id={$this->library->document_library_id}");
		}
		
		
		function deleteFile()
		{
			if (confirm("Are you sure you want to delete this file?"))
			{
				id = $('files_value').value;
				go("document_delete.php?doc_id=" + id);
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
				form.download.disabled = true;
				form.edit.disabled = true;
				form.deleteButton.disabled = true;
			}
		}
		
		
		</script>
		
ENDSCRIPT;
		return $script;
	}
	
	function drawView()
	{
		global $config;
		
		$tree = new DirectoryTreeControl("files");
		$tree->width = 500;
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
}

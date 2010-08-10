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
		$script .= <<<ENDSCRIPT
		
		<script type="text/javascript">
		
		function downloadFile()
		{
			id = $('files_value').value;
			go("/action/document/download?document_id=" + id);
		}
		
		function editFile()
		{
			id = $('files_value').value;
			go("document_form.php?doc_id=" + id);
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
		<button id="upload" class="button" onclick="go('document_form.php')">Upload a File...</button>
<?
	}
}

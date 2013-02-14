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

class DocumentSerializationHandler
{
	function DocumentSerializationHandler()
	{
	}


	function export()
	{
		$xml  = "\n<DocumentMap>";
		$xml .= SerializationManager::serialize(DocumentLibrary, "ORDER BY document_library_id");
		$xml .= SerializationManager::serialize(Document, "ORDER BY document_id");
		$xml .= "</DocumentMap>";
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(DocumentLibrary, $doc, $tx);
		SerializationManager::unserialize(Document, $doc, $tx);		
	}			
}
	
class DocumentManager
{
	var $library;
	var $docs;
	var $documents;
	var $width = 700;
	var $height = 400;
	
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
		
		if (strpos($folder, "Board Members Only") !== FALSE && (!checkRole("board,admin,super") && !$this->library->allowManageFiles()))
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
<div class="dialog" id="documentManagerDlg" style="width: 570px">
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

function uploadResult(responseText)
{
	if (responseText == "1") 
	{
		window.location.reload();
	}
	else
	{
		alert(responseText);
	}
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
		$('download').removeProperty("disabled");
		if($('edit')) $('edit').removeProperty("disabled");
		$('deleteButton').removeProperty("disabled");
		
	}
	else
	{
		$('download').addProperty("disabled", "disabled");
		if($('edit')) $('edit').addProperty("disabled", "disabled");
		$('deleteButton').addProperty("disabled", "disabled");
	}
}

function rescanLibrary()
{
	result = httpRequest("/action/document/rescan?document_library_id=<?echo $this->library->document_library_id?>");
	if (result == "OK")
	{
		window.location.reload();
	}
	else
	{
		alert(result);
	}
}

<?
		if (checkRole($this->library->manage_folders) || $this->library->allowManageFiles())
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
		$tree->width = $this->width;
		$tree->height = $this->height;
		$tree->onSelectionChanged = "selectionChanged";
		$tree->onDoubleClick = "downloadFile";
		
		$tree->selectMode = "file";
		$tree->permissionCallback = array($this, folderPermission);
		
		$root = $this->library->getLibraryDirectory();
		
		$tree->buildFolderTree($config["uploaddir"], $root);
				
		foreach($this->documents as $doc)
		{
			// Do not list files to which the user does not have access
			if (!checkRole($doc->allow_access)) continue;
			
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
	
			$tree->writeHTML(); 

			$this->drawButtons();
		}
		
		function drawButtons()
		{
?>		
		<br/>
		<button id="download" class="button" disabled="disabled"  onclick="downloadFile()"> Download File </button>&nbsp;&nbsp;
		<?
		if (checkRole($this->library->upload_files))
		{
?>
		<button id="edit" class="button" disabled="disabled" onclick="editFile()">Edit Details...</button>&nbsp;&nbsp;
		<button id="upload" class="button" onclick="uploadFile()">Upload a File...</button>
<?
		}
		
		if (checkRole($this->library->delete_files))
		{
?>
		<button id="deleteButton" class="button" disabled="disabled" onclick="deleteFile()">Delete File</button>&nbsp;&nbsp;
<?
		}
		
		if (checkRole($this->library->manage_folders))
		{
?>
		&nbsp;&nbsp;<button id="createFolder" class="button" onclick="createFolder()">Create Folder...</button>&nbsp;&nbsp;
		<button id="deleteFolder" class="button" onclick="deleteFolder()">Delete Folder</button>&nbsp;&nbsp;
<?php			
		}

		if (checkRole("admin"))
		{
?>
		<button id="rescan" class="button" onclick="rescanLibrary()">Rescan</button>
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
			throw new FakoliException("No upload record found.");
		}
		
		if ($_FILES[$field]["name"]=="") 
		{
			throw new FakoliException("Upload name is empty");
		}
		
		/* Copy across the uploaded file */
		$folder = str_replace("/", DIRECTORY_SEPARATOR, $folder);
		$folder = str_replace("\\", DIRECTORY_SEPARATOR, $folder);
		
		$name = $_FILES[$field]["name"];
		
		if (!$folder) $folder = "documents";
		
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
	
	/*
	 * Delete a folder record and its path in the file system.
	 */
	function deleteFolder($folder)
	{
		$folder = sanitizePath($folder);
		$path = $this->library->getLibraryDirectory() . DIRECTORY_SEPARATOR . $folder;
	
		if (file_exists($path))
			rmdir($path);
		$folder->delete();
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
					trace("Found Folder {$folderNode->id} for document {$doc->document_id}", 4);
					$id = str_replace(DIRECTORY_SEPARATOR, "_", $file);
					$fileNode = new FileNode($id, basename($file), $doc->document_id);
				
					$folderNode->add($fileNode);
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
	
		
	/**
	 * Enumerate the Document objects.
	 * @param  $items collection that the Document items are returned in
	 */
	static function enumerateItems($items)
	{
		$documents = Query::create(DocumentLibrary, "ORDER BY name")->execute();
		
		$items["Document Libraries"] = $documents;
		return $items;
	}
	
	static function displayLibrary($identifier, &$continue)
	{
		try
		{
			$library = DocumentLibrary::findByIdentifier($identifier);			
			$page = ComponentPage::findByIdentifier("document_library", "WHERE enabled=1");
			
			$_GET["library"] = $identifier;
		
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		
			$page_role = $page->role;
			
			if (!checkRole($page->role))
			{
				LoginManager::storeRedirectPage();
				redirect("/login");
			}
			
			echo $pageView->drawView();
			
			$continue = false;
		}
		catch(DataNotFoundException $e)
		{
			
		}
		
		return $identifier;
	}
	
	// Used by document_library_form and fileshare_library_form
	static function createLibraryObj()
	{
		global $user;

		$library = new DocumentLibrary();
		$library->owner_id = $user->user_id;
		$library->create_date = now();
		$defaultRole = DocumentLibrary::getDefaultRole();
		if($defaultRole)
		{
			$library->allow_access = $defaultRole;
			$library->manage_folders = $defaultRole;
			$library->upload_files = $defaultRole;
			$library->delete_files = $defaultRole;
		}	
		
		
		$library->filter = new InclusionFilter("document_library_id", "name", 
		"identifier", "description", "owner_id", "last_modified",
		"allow_access", "manage_folders", "upload_files", "delete_files", "create_date");
		
		return $library;
	}	
	
	static function createLibrary($form)
	{
		$d = $form->data->getLibraryDirectory();
		if (!is_dir($d) && !file_exists($d))
		{
			$docs = $d.DIRECTORY_SEPARATOR."documents";
			mkdir($d);
			mkdir($docs);
			chmod($docs, 0777);
		}
	}
	
	function deleteDocumentLibrary()
	{
		$documents = $this->library->Documents();
		if(count($documents) > 0)
		{
			foreach($documents as $document)
				$this->deleteDocument($document);
		}
			
		$folders = $this->library->Folders();
		if(count($folders) > 0)
		{
			foreach($folders as $folder)
			{
				$this->deleteFolder($folder);
			}
		}
			
		$fileshareUsers = $this->library->FileshareUsers();
		if(count($fileshareUsers) > 0)
		{
			foreach($fileshareUsers as $f_user)
				$f_user->delete();
		}	
		
		$this->library->delete();
	}

	// Not the most efficient but how often will we use it?
	static function deleteRole($del_role)
	{
		$constraint = "WHERE role like '%$del_role%'";
		
		$roleFields = array("upload_files", "delete_files", "manage_folders", "allow_access");
		foreach($roleFields as $field)
		{
			$libraries = Query::create(DocumentLibrary, "WHERE $field like '%$del_role%'")->execute();
			RoleManager::deleteRoleFromString($libraries, $del_role, $field);
		}
		
		return $del_role;	
	}		
	
	static function documentLibraryTabs($key)
	{
		$tabs = array(	
				"Details"		=>	"document_library_form",
				"Files"			=>	"document_library_files"
					);

		$qs = ($key) ? "document_library_id=$key" : ""; 
		return new TabBar("tabs", $tabs, $qs);
	}	
	
	static function buildDocumentLibraryForm($library)
	{
		$form = new AutoForm($library);
		$form->hide("owner_id", "allow_access", "manage_folders", "upload_files", "delete_files", "create_date");
		$form->required("name", "identifier");
		$form->allowDelete = true;
		$form->onDelete = deleteDocumentLibary;
		
		return $form;
	}	

	static function getStyles()
	{
		$styles .= "<link href=\"/fakoli/css/tabs.css\" rel=\"stylesheet\"/>\n";
		
		return $styles;
	}	
	
	function rescan()
	{
		$this->indexedDocuments = reindexList($this->documents, "file");
		$this->indexedFolders = IndexedQuery::create(DocumentFolder, "WHERE document_library_id=:id", "path")
											->bind(":id", $this->library->document_library_id)
											->execute();
											
		$this->scanFiles("documents");											
	}
	
	function scanFiles($path)
	{
		global $user;
		
		$base = sanitizePath($this->library->getLibraryDirectory() . DIRECTORY_SEPARATOR . $path);
		
		$files = scandir($base);
		foreach($files as $file)
		{
			if ($file == "." || $file == "..") continue;
			 
			$f = $path . DIRECTORY_SEPARATOR . $file;
							
			if (is_dir($base . DIRECTORY_SEPARATOR . $file))
			{
				$this->scanFiles($f);
			}
			else
			{
				if ($this->indexedDocuments[$f]) continue;
				
				$doc = new Document();
				$doc->title = $file;
				$doc->file = $f;
				$doc->keywords = "";
				$doc->notes = "";
				$doc->public = true;
				$doc->document_library_id = $this->library->document_library_id;
				$doc->owner_id = $user->get($user->getPrimaryKey());
				$doc->save();
			}
		}
	}
	
	function importMetaData($file) 
	{
		$this->indexedDocuments = reindexList($this->documents, "file");
		
		$fp = fopen($file, "r");
		
		$fields = fgetcsv($fp);
		
		$fileIdx = -1;
		
		foreach($fields as $field)
		{
			if ($field == "file")
			{
				$fileIdx = $i;
				break;
			}
			
			$i++;
		}

		if ($fileIdx < 0)
		{
			throw new FakoliException("'field' column not present");
		}
		
		while($values = fgetcsv($fp))
		{
			$files = explode(";", $values[$fileIdx]);
			foreach($files as $f)
			{
				$f = trim($f);
				
				trace($path, 3);
				
				foreach($this->indexedDocuments as $path => $doc)
				{
					if (basename($path) == $f)
					{
						$i = 0;
						
						foreach($fields as $field)
						{
							if ($field == "file")
							{
								$i++;
								break;
							}
							
							$doc->set($field, $values[$i]);
							$i++;
						}
					
						$doc->save();
					}
				}
			}
		}
	}
	
	/*
	 * Read  or allow_access roles, include
	 * visitor (nonuser) option.
	 */
	static function getRoleOptions()
	{
		$roles = SiteRole::getRolesArray();
		$roles["visitor"] = "Visitor";	
		return $roles;
	}

	static function getRoleList()
	{
		$roles = DocumentManager::getRoleOptions();
		return implode(",", array_keys($roles));
	}	
	
	static function registerSearchables($searchables)
	{
		trace("DocumentManager registerSearchables", 3);
		if (!Settings::getSetting("solr", "enabled"))
		{
			$searchables["document"] = array(Document);
		}
		return $searchables;
	}

	static function upgradeComponent($version)
	{
		$mgr = new DocumentUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	static function registerTaxonomyClasses($classes)
	{
		$classes[] = Document;
		return $classes;
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("document_libraries", "Document Libraries &amp; Document Records (files need to be transferred manually)", new DocumentSerializationHandler());
		return true;
	}
}

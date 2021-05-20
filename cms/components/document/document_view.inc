<?php
/**\addtogroup document
 * @{
 */


Fakoli::using("document");
Fakoli::usingFeature("facet_manager");

class DocumentLibraryView
{
	var $mgr;
	var $library;
	var $table;
	var $facetManager;
	
	function __construct($mgr)
	{
		$this->mgr = $mgr;	
		$this->library = $mgr->library;
		$this->table = $this->buildDocumentTable();
		
		$this->facetManager = new FacetManager('facet_manager', $this->table);
		TaxonomyManager::addFacets($this->facetManager, Document, true, "160px", "200px", "200px");
		
		if (Settings::getValue("document", "show_upload_details"))
		{
			$this->facetManager->addFacet("Uploaded By", $this->createUploadedByFacet());
		}
	}
	
	function buildDocumentTable()
	{
		$showUploadDetails = Settings::getValue("document", "show_upload_details");
		
		$docWidth = $showUploadDetails ? "40%" : "80%";
		
		$table = new DataListView($this->mgr->documents, "document_library");
		$table->column("Document", array($this, formatDocumentTitle), true, "width: {$docWidth}", String, null, "{title:lower}")
			  ->column("Author", "{author}", true, "width: 20%");
		
		if ($showUploadDetails)
		{
			$table->column("Uploaded By", "{Owner.first_name} {Owner.last_name}", false, "width: 20%; text-align: center")
				  ->column("Last Modified", "{last_modified:short}", false, "width: 30%; text-align: center");
		}
		
		$table->pageSize = 10;
		$table->emptyMessage = "There are no matching documents";
		$table->sortable = true;
		$table->filter = Settings::getValue("document", "show_table_filter") ? true : false;
		
		$table->writeIdTag = true;
		
		return $table;
	}
	
	function formatDocumentTitle($document)
	{
		$file = $document->file;
		
		$type = getDocType($file);
		$icon = getDocIcon($file);
		
		$size = $this->library->getFileSize($document);
		
		if(checkRole($this->library->upload_files))
		{
			$details_icon = "icon_edit.png";
			$fn = "editDocumentDetailsDialog";
		}
		else
		{
			$details_icon = "icon_view.png";
			$fn = "showDocumentDetailsDialog";
		}

		$titleText = Settings::getValue("document", "link_to_details") ? "<a class='document_title' href='document_details?document_id={document_id}'>{title}</a>" :
																		 "<span class='document_title'>{title}</span>";
		
		$details_link  = "<a href=\"edit_document?document_id={document_id}\" onclick=\"documentMgr.{$fn}({document_id}); return false;\"><img src=\"/fakoli/images/{$details_icon}\" style=\"border: none;display: inline-block;vertical-align: middle;padding-right: 4px\"></a>\n";
			
		if(checkRole($this->library->delete_files))
		{
			$delete_link .= "<a href=\"delete_document?document_id={document_id}\" onclick=\"documentMgr.deleteFile({document_id}); return false;\"><img alt=\"delete\" src=\"/fakoli/images/icon_delete_trash.png\"></a>\n";
		}
		
		return $document->format("$titleText<br/><img style='display:inline-block;vertical-align: middle' src='$icon' alt='$type'/> <a href='/action/document/download?document_id={document_id}'>Download</a>&nbsp;$size&nbsp;<span style='display: inline-block'>&nbsp;{$details_link}&nbsp;{$delete_link}</span>");
	}
	
	function writeScript()
	{
		$script .= $this->facetManager->writeScript();
		$script .= $this->table->writeScript();
		
		ob_start();
		?>
		<script type="text/javascript" src="/components/document/js/document_manager.js"></script>
		<script type="text/javascript">
		var documentMgr;
		
		window.addEvent('domready', function()
		{
			documentMgr = new DocumentManager(<?php echo $this->library->document_library_id ?>);
		});

		// For compatibility with tree view - this function called from edit.inc
		function editResult(result)
		{
			documentMgr.editResult(result);
		}

		// For compatibility with tree view - this function called from upload.inc
		function uploadResult(responseText)
		{
			documentMgr.uploadResult(responseText);
		}	
		</script>
		<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}
	
	function drawView()
	{
		$this->facetManager->drawForm();
		$this->table->drawView();
		
		$this->drawButtons();
	}
	
	function drawButtons()
	{
		global $isAdmin;
		
		if(!checkRole("admin") && !checkRole($this->library->upload_files))
		{
			return;
		}
		
		echo "<div id='document_library_button_row'>\n";
		if (checkRole($this->library->upload_files))
		{
			echo "<button id='upload' class='button' onclick='documentMgr.uploadFile()'>Upload a File...</button>\n";
		}
				
		if ($isAdmin && checkRole("admin"))
		{
			echo "<button id='rescan' class='button' onclick='documentMgr.rescanLibrary()'>Rescan</button>\n";
		}
		
		echo "</div>\n";
	}
	
	function createUploadedByFacet()
	{
		$facet = new MultiSelectFacetFilter("owner_id", array(Document	=>	"{Owner.first_name} {Owner.last_name}"));
	
		$facet->dropdownMaxWidth = "160px";
		$facet->width = "200px";
		$facet->height = "240px";
	
		return $facet;
	}
	
}

class DocumentLibraryListView
{
	var $table;
	
	function DocumentLibraryListView($libraries)
	{
		$this->table = $this->buildTable($libraries);
	}
	
	function buildTable($libraries)
	{
		$table = new DataListView($libraries, "libraries");
		$table->column("Library", array($this, formatTitleLink), true, "width: 50%")
			  ->column("Identifier", "{identifier}", true, "width: 15%")
	  		->column("Owner", "{getOwnerName()}", true, "width: 20%")
	  		->column("# Docs", "{countDocuments()}", true, "width: 15%;text-align: right");
	  
		$table->sortable = true;
		$table->filter = true;
		$table->pageSize = 10;
		$table->emptyMessage = "No Document Libraries have been created.";
			
		return $table;
	}
	
	function formatTitleLink($library)
	{
		return $library->format("<a href='document_library_form?document_library_id={document_library_id}'>{name}</a>{description}");	
	}
	
	function writeScript()
	{
		return $this->table->writeScript();
	}
	
	function drawView()
	{
		$this->table->drawView();
	}
}

class AdminDocumentLibraryListView extends DocumentLibraryListView
{
	function AdminDocumentLibraryListView($libraries)
	{
		parent::DocumentLibraryListView($libraries);
	}
	
	function formatTitleLink($library)
	{
		return $library->format("<a href='/admin/document_library_form?document_library_id={document_library_id}'>{name}</a>{description}");	
	}
	
}
/**@}*/?>
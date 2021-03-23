<?php
/**\addtogroup document
 * @{
 */


Fakoli::using("document");

class DocumentFileView
{
	var $id;
	var $library;
	
	function __construct($library, $id = "documents")
	{
		$this->id = $id;
		$this->library = $library;
	}
	
	function writeScript()
	{
		ob_start();
		?>
		<script type="text/javascript">
		function editFile(id)
		{
			modalPopup("Edit Document Properties",  "/action/document/edit?document_id=" + id, 900, 'auto');
		}
		
		function addFile()
		{
			modalPopup("Add a New Document", "/action/document/upload?document_library_id=<?echo $this->library->document_library_id?>", 900, 'auto');
		}
		
		function editResult(result)
		{
			if (result == "1")
			{
				window.location.reload();
			}
			else
			{
				document.id('Document_form__error').set({'text': result, 'display': 'table-cell'});
			}
		}
		
		function uploadResult(result)
		{
			if (result == "1")
			{
				window.location.reload();
			}
			else
			{
				document.id('Document_form__error').set({'text': result, 'display': 'table-cell'});
			}
		}

		function moveDocuments(document_library_id)
		{
			var table = document.id(<?php echo $this->id?>);
			var document_ids = [];
			table.getElements("input[type=checkbox]:checked").each(function(cbox)
			{
				document_ids.push(cbox.value);
			});
				
			document_ids = document_ids.join(",");
			modalPopup("Move Documents", "/action/document/move_documents?document_library_id=" + document_library_id + "&document_ids=" + document_ids, "700px", "auto", true);
		}

		function moveDocumentsResponse(response)
		{
			if (response == "OK")
			{
				window.location.reload();
			}
			else
			{
				notification(response);
			}
		}

		function deleteDocuments(document_library_id)
		{
			if (confirm("Are you sure you want to delete the selected documents?\nThis operation cannot be undone."))
			{
				var table = document.id(<?php echo $this->id?>);
				var document_ids = [];
				table.getElements("input[type=checkbox]:checked").each(function(cbox)
				{
					document_ids.push(cbox.value);
				});
					
				document_ids = document_ids.join(",");
				go("/action/document/delete_documents?document_library_id=" + document_library_id + "&document_ids=" + document_ids);
			}
		}
		
		</script>
		<?
		$script = ob_get_contents();
		ob_end_clean();
		
		return $script;
	}
	
	function drawView()
	{
		global $script;
		
		$documents = $this->library->Documents("ORDER BY title DESC");
		
		$table = new DataListView($documents, $this->id);
		$table->selector();
		$table->column("ID", "{document_id}", true);
		$table->column("Title", "<b><a href='#' onclick='editFile({document_id});return false'>{title}</b></a>", true, "width: 40%")
			  ->column("Author", "{author}", true, "width: 35%")
			  ->column("Last Modified", "{last_modified}", true, "width: 25%;text-align: center");
			  
		$table->filter = true;
		$table->sortable = true;
		$table->pageSize = 20;
		$table->excelFile = "documents";
		$table->emptyMessage = "No Documents have been added yet.";
		
		
		$table->showSelectAll = true;
		$contextMenu = $table->commandMenu();
		$contextMenu->command("move_documents", "Move Documents...", "moveDocuments({$this->library->document_library_id})", false);
		$contextMenu->command("delete_documents", "Delete Documents", "deleteDocuments({$this->library->document_library_id})", false);
		
		$script .= $this->writeScript();
		$script .= $table->writeScript();
		$table->drawView();
?>		
		<br/>
		<br/>
		<a href="#" class="button" onclick="addFile(); return false"> Add a New Document </a>
		<a href='#' class='button' onclick='moveDocuments(); return false'> Move Documents... </a>
		<a href='#' class='button' onclick='deleteDocuments(); return false'> Delete Documents </a>
		<br/>
<?		
	}
}
/**@}*/?>
<?php
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
			modalPopup("Edit Document Properties",  "/action/document/edit?document_id=" + id, 900, 600);
		}
		
		function addFile()
		{
			modalPopup("Add a New Document", "/action/document/upload?document_library_id=<?echo $this->library->document_library_id?>", 900, 600);
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
		$table->column("Title", "<b><a href='#' onclick='editFile({document_id});return false'>{title}</b></a>", true, "width: 40%")
		->column("Author", "{author}", true, "width: 35%");
		
		$table->filter = true;
		$table->sortable = true;
		$table->pageSize = 20;
		$table->excelFile = "documents";
		$table->emptyMessage = "No Documents have been added yet.";

		$script .= $this->writeScript();
		$script .= $table->writeScript();
		$table->drawView();
?>		
		<br/>
		<br/>
		<a href="#" class="button" onclick="addFile(); return false"> Add a New Document </a>
		<br/>
<?		
	}
}
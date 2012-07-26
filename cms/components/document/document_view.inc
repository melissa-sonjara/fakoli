<?php

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
		$table->column("Library", array($this, formatTitleLink), true, "width: 55%")
	  		->column("Owner", "{getOwnerName()}", true, "width: 30%")
	  		->column("# Docs", "{countDocuments()}", true, "width: 15%;text-align: right");
	  
		$table->sortable = true;
		$table->filter = true;
		$table->pageSize = 10;
		$table->emptyMessage = "No Document Libraries have been created.";
			
		return $table;
	}
	
	function formatTitleLink($library)
	{
		return $library->format("<a href='/document_library_form?document_library_id={document_library_id}'>{name}</a>{description}");	
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
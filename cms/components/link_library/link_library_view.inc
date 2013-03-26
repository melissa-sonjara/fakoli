<?php

class LinkLibraryListView
{
	var $table;
	
	function LinkLibraryListView($libraries)
	{
		$this->table = $this->buildTable($libraries);
	}
	
	function buildTable($libraries)
	{
		$table = new DataListView($libraries, "libraries");
		$table->column("Library", array($this, formatTitleLink), true, "width: 55%")
	  		->column("Owner", "{getOwnerName()}", true, "width: 30%")
	  		->column("# Links", "{countLinks()}", true, "width: 15%;text-align: right");
	  
		$table->sortable = true;
		$table->filter = true;
		$table->pageSize = 10;
		$table->emptyMessage = "No Link Libraries have been created.";
			
		return $table;
	}
	
	function formatTitleLink($library)
	{
		return $library->format("<a href='/link_library_form?link_library_id={link_library_id}'>{name}</a>{description}");	
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

class AdminLinkLibraryListView extends LinkLibraryListView
{
	function AdminLinkLibraryListView($libraries)
	{
		parent::LinkLibraryListView($libraries);
	}
	
	function formatTitleLink($library)
	{
		return $library->format("<a href='/admin/link_library_form?link_library_id={link_library_id}'>{name}</a>{description}");	
	}
	
}
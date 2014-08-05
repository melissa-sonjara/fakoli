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



abstract class LinkLibraryView
{
	var $library;
	var $view;
	
	function __construct($library)
	{
		$this->library = $library;
		$this->links = $this->library->Links("WHERE published=1 ORDER BY sort_order, title, url");
		$this->view = $this->buildView();
		
		$this->facetManager = new FacetManager('facet_manager', $this->view);
		TaxonomyManager::addFacets($this->facetManager, LinkRecord, true, "160px", "200px", "200px");
	}

	static function create($library)
	{
		$format = Settings::getValue("link_library", "display_format");
		
		switch($format)
		{
			case "Tabular":
				return new TabularLinkLibraryView($library);
				
			default:
				return new ListLinkLibraryView($library);
		}
	}

	abstract function buildView();
	
	abstract function drawView();
}


class TabularLinkLibraryView extends LinkLibraryView
{
	function __construct($library)
	{
		parent::__construct($library);
	}
	
	function buildView()
	{
		$table = new DataListView($this->links, "library_link_list");
		$table->column("Link", array($this, formatLinkTitle), true, "width: 40%", String, null, "{title:lower}")
			  ->column("Description", "{description}", true, "width: 40%")
			  ->column("Last Modified", "{last_modified:short}", false, "width: 20%; text-align: center");
				
		$table->pageSize = 10;
		$table->emptyMessage = "No links have been added to this library.";
		$table->sortable = true;
		$table->filter = false;
	
		$table->writeIdTag = true;
	
		return $table;
	}

	function formatLinkTitle($link)
	{
		if(checkRole($this->library->write_access))
		{
			$details_icon = "icon_edit.png";
			$fn = "LinkLibraryManager.editLink";
			$details_link  = "<a href=\"#\" onclick=\"LinkLibraryManager.editLink({link_id}); return false;\"><img src=\"/fakoli/images/{$details_icon}\" style=\"border: none;display: inline-block;vertical-align: middle;padding-right: 4px\"></a>\n";		
		}
	
		if(checkRole($this->library->write_access))
		{
			$delete_link .= "<a href=\"#\" onclick=\"LinkLibraryManager.deleteLink({link_id}); return false;\"><img alt=\"delete\" src=\"/fakoli/images/icon_delete_trash.png\"></a>\n";
		}
	
		return $link->format("<span class='document_title'>{title}</span><br/><img style='display:inline-block;vertical-align: middle' src='/fakoli/images/file_icon.png' alt='External Link'/> <a href='{url}'> Read More </a><span style='display: inline-block'>&nbsp;{$details_link}&nbsp;{$delete_link}</span>");
	}

	function drawView()
	{
		global $script;
		
		$script .= $this->facetManager->writeScript();
		$script .= $this->view->writeScript();
		
		$this->facetManager->drawForm();
		$this->view->drawView();
		
		$this->drawButtons();
	}
		
	function drawButtons()
	{
		if(!checkRole("admin") && !checkRole($this->library->upload_files))
		{
			return;
		}
		
		echo "<div id='document_library_button_row'>\n";
		if (checkRole($this->library->upload_files))
		{
			echo "<button id='upload' class='button' onclick='LinkLibraryManager.addLink({$this->library->link_library_id});'>Add a Link...</button>\n";
		}
		
		echo "</div>\n";
	}	
}

class ListLinkLibraryView extends LinkLibraryView
{
	function __construct($library)
	{
		parent::__construct($library);
	}
	
	function buildView()
	{
		$list = new PagedList($this->links, "library_links", "{LinkLibraryManager::formatLink}");
		$list->pageSize = 20;
		$list->emptyList = "No links have been added to this library";

		return $list;
	}
	
	function drawView()
	{
		global $script;
		
		$script .= $this->view->writeScript();
		$this->view->drawList();
	}
}

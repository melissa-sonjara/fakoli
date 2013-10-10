<?php

class OnlineHelpHelper
{
	
	static function formatHelpPageContent($helpPage)
	{
		return ellipsis(stripHTML($helpPage->content), 100, true);
	}
	
	static function formatParentOptions($helpBook, $helpPage)
	{
		if($helpPage->identifier)
		{
			$constraint = "WHERE identifier != '{$helpPage->identifier}'";
		}
		$helpPages = $helpBook->Pages("$constraint ORDER BY sort_order");
		$options = array();
		
		$options[0] = "";
		if(count($helpPages))
		{
			foreach($helpPages as $hp)
			{
				if($helpPage->identifier && $hp->isChildOf($helpPage)) continue;
				$options[$hp->help_page_id] = $hp->title;
			}
		}	
	
		return $options;
	}
	
	static function formatSubpages($helpPage)
	{
		$children = $helpPage->Children();
		
		if(count($children))
		{
			return formatItems($children, "<a style='display: inline' href='help_page_form?help_book_id={help_book_id}&help_page_id={help_page_id}'>{title}</a>", ", ");
		}
		
		return "None";
	}
	
	static function buildRootPageTable($pages, $help_book_id)
	{
		$nodes = array();
		if(count($pages))
		{
			foreach($pages as $page)
			{
				if(!$page->parent_id)
				{
					$nodes[] = $page;
				}
			}
		}
		
		return OnlineHelpHelper::buildPageTable($nodes, $help_book_id);
	}
	
	static function buildPageTable($pages, $help_book_id)
	{
		$table = new DataListView($pages, "help_pages");
		$table->column("Help Page", "<a href='help_page_form?help_page_id={help_page_id}&help_book_id={help_book_id}'>{title}</a>")
			->column("Identifier", "{identifier}", false, "width: 25%")
			->column("Content", array(OnlineHelpHelper, formatHelpPageContent), false, "width: 30%")
			->column("Subpages", array(OnlineHelpHelper, formatSubPages), false, "width: 20%")
			->column("Public", "{public:Yes/No}", false, "text-align: center; width: 15%")
			;
		
		if(count($pages) > 1)
		{
			$table->enableDragReorder("/action/online_help/reorder_help_pages?help_book_id=$help_book_id");
			$table->dragText = "<span style='font-size: 10px'>Click and drag to change the order of the help pages</span>";
		}
		$table->filter = false;
		$table->sortable = false;
		$table->paginate = false;	

		return $table;
	}
}
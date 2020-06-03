<?php

class SharingContentManager implements ISectionContentManager
{
	function getDefaultPage($section) { return $section->default_page; }
	
	function getContent($section, $identifier)
	{
		// Create Mock SectionContent object
		$content = new SectionContent();
		$content->section_id = $section->section_id;
		$content->identifier = $identifier;
		return $content;
	}
	
	function sendContent($section, $identifier)
	{
		if (!$identifier)
		{
			throw new FakoliException("Missing identifier");
		}

		Fakoli::put("share_token", $identifier);
		$page = ComponentPage::findByIdentifier('shared_view', "WHERE enabled=1");
		$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		echo $pageView->drawView();
	}
	
	function getAdminView($section)
	{
		return new SharingContentAdminView($section);
	}
}

class SharingContentAdminView extends AbstractSectionContentAdminView
{
	function __construct($section)
	{
		parent::__construct($section);
	}
	
	function handlePOST()
	{
	}
	
	function drawView()
	{
		global $script;
		
		$tokens = Query::create(ShareToken, "ORDER BY created_date")->execute();
		$table = new DataListView($tokens, "share_tokens");
		$table->column("Token", "<a href=''>{token}</a>", true)
			  ->column("Type", "{item_type}", true)
			  ->column("Shared By", "{User.getFullName()}", true)
			  ->column("Active", "<i class='fa-fw fas fa-{active:check/times}'></i>", true, "text-align: center")
			  ->column("# Accesses", "{countAccesses()}", true, "text-align: right");
			  
		$table->pageSize = 20;
		$table->filter = true;
		$table->sortable = true;
		$table->emptyMessage = "No items have been shared";
		$table->excelFile = "share_tokens.xls";
		
		$script .= $table->writeScript();

		echo "<h3>Share Tokens</h3>";
		$table->drawView();
	}
}

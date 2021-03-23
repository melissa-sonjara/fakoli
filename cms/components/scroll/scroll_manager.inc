<?php
/**\addtogroup scroll
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the scroll component.
 */

class ScrollManager
{
	function ScrollManager()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("scroll", "scroll_part_template", "<a name='{title:codify}'></a>\n<div id='{title:codify}' class='part {css_class}' style='{styles}'>\n<div class='content'>{content}</div>\n</div>", Text);
	}

	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("scroll", "Scrolls and Parts", new CompoundSerializationHandler("ScrollMap", 'Scroll', 'ScrollPart'));
	}
	
	static function scrollTabs($key)
	{
		$tabs = array(	"Definition"	=>	"scroll_form",
						"Parts"			=>	"scroll_parts"
		);
	
		$qs = ($key) ? "scroll_id=$key" : "";
		return new TabBar("tabs", $tabs, $qs);
	}
	

	static function displayScroll($identifier, &$continue)
	{
		try
		{
			trace("Searching for scroll with identifier '$identifier'", 2);
			$scroll = Scroll::findByIdentifier($identifier);
			$page = ComponentPage::findByIdentifier("scroll", "WHERE enabled=1");
			Fakoli::put("content_object", $scroll);
		
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

	static function enumerateScrolls($items)
	{
		$scrolls = Query::create(Scroll, "ORDER BY identifier")->execute();
		
		$items["Scrolls"] = $scrolls;
		return $items;
	}

	static function registerTaxonomyClasses($classes)
	{
		$classes[] = Scroll;
		return $classes;
	}
	
	static function registerVersionedContent()
	{
		VersionedContentManager::registerVersionedContent(ScrollPart);
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new ScrollUpgradeManager();
		$mgr->upgrade($version);
	}
}

/**@}*/?>
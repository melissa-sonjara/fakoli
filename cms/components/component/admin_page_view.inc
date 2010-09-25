<?php

Fakoli::using("component");

class AdminPageView
{
	var $identifier;
	var $page;
	
	function AdminPageView($identifier)
	{
		if (!$identifier) $identifier = "index";
		$this->identifier = $identifier;
		
		$this->page = Cache::get("fakoli_admin_pages_$identifier");
		if ($this->page) return;
		
		$pages = Query::create(AdminPage, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->execute();
					  
		if (count($pages) != 1)
		{
			throw new FakoliException("Missing or ambiguous page identifier '$identifier'");
		}
		
		$this->page = $pages[0];
		Cache::put("fakoli_admin_pages_$identifier", $this->page);
	}


	function drawView()
	{
		global $config;
		global $method;
		global $user;
		global $script;
		global $styles;
		global $dialogs;
		global $page;
		global $menu_item;
		global $isAdmin;
		
		$page = $this->page; // Pass through to code include
		
		$template = file_get_contents(Fakoli::resolveResource("/fakoli/admin.tpl"));
		
		ob_start();
		
		try
		{
			include $page->server_path;
		}
		catch(Exception $e)
		{
			//
		}
		
		$content = ob_get_contents();
		ob_end_clean();
	
		$admin_menu = Fakoli::getAdminMenu();
		
		$menu = $this->formatMenu($admin_menu);
		
		// If the script didn't provide a title, guess a reasonable default.
		
		if (!$title) $title = prettify(basename(substr($page->server_path, 0, -4)));
		
		// Pull in the script and style dependencies defined by the components
		
		$script = Fakoli::getScripts() . $script;
		$styles = Fakoli::getStyles() . $styles;
		
		// Populate variable references
		
		$vars = array();
		
		preg_match_all("/\{var:(.*?)\}/", $template, $vars, PREG_SET_ORDER);
		
		foreach($vars as $var)
		{		
			$expr = $var[0];
			$name = $var[1];	
			$template = str_replace($expr, $$name, $template);
		}
		
		if (!$config['prettyURLs'])
		{
			$template = preg_replace("/(href|src|action)=(['\"])\\/?([\\w\\d_]+?)\\?/i", "$1=$2/page.php?identifier=$3&", $template);
			$template = preg_replace("/(href|src|action)=(['\"])\\/?([\\w\\d_]+?)[\"']/i", "$1=$2/page.php?identifier=$3$2", $template);
			$template = preg_replace("/go\\(([\"'])\\/?([\\w\\d_]+?)\\?/", "go($1/page.php?identifier=$2&", $template);
			$template = preg_replace("/go\\(([\"'])\\/?([\\w\\d_]+?)[\"']/", "go($1/page.php?identifier=$2$1", $template);
			
		}
				
		$template = ComponentManager::fireEvent("PostProcessPage", &$template);
		
		echo $template;		
	}

	function formatMenu($admin_menu)
	{	
		global $menu_item;
		
		if (!$admin_menu) return "";
			
		ob_start();
		
		if (!$menu_item) $menu_item = basename($_SERVER['PHP_SELF']);
?>
<div id="menu">
<?
		$menuSectionIdx = 0;
		
		foreach($admin_menu as $section => $items)
		{
?>
 <h4 class="toggler atStart"><?echo $section ?></h4>
 <div class="atStart">
<?			
			foreach($items as $item => $properties)
			{
				if ($item == $menu_item) 
				{
					$class = 'selectedadminitem';
					$selectedMenuSectionIdx = $menuSectionIdx;
				}
				else
				{
					$class = 'adminmenuitem';
				}
				if (checkRole($properties["role"]))
				{
					echo "<a class='$class' href='{$properties['page']}{$menu_params}'>{$item}</a>\n";
				}
			}
			
			++$menuSectionIdx;
?>
</div>
<?
		}
?>
</div>
<?
		if (!$selectedMenuSectionIdx) $selectedMenuSectionIdx = 0;

		global $script;
		
		$script .= <<<ENDSCRIPT

<script type="text/javascript">
window.addEvent('domready', function() {

	var accordion = new Accordion('h4.atStart', 'div.atStart', {

		opacity: false,
		initialDisplayFx: false,
		display: $selectedMenuSectionIdx,
		onActive: function(toggler, element){
			toggler.setStyles({'background-image': 'url(/fakoli/images/section_open.png)'});
		},
	 
		onBackground: function(toggler, element){
			toggler.setStyles({'background-image': 'url(/fakoli/images/section_closed.png)'});
		}
	}, $('accordion'));

	});
</script>
ENDSCRIPT;
		$menu = ob_get_contents();
		ob_end_clean();
		return $menu;
	}
}

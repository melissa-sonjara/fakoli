<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/** \addtogroup component
 * @{
 */

class AdminPageView
{
	var $identifier;
	var $page;
	
	function __construct($identifier)
	{
		if (!$identifier) $identifier = "index";
		$this->identifier = $identifier;
		
		$this->page = Cache::get("fakoli_admin_pages_$identifier");
		if ($this->page) return;
		
		$pages = Query::create('AdminPage', "WHERE identifier=:id")
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
		global $fontawesome;
		
		global $auto_form_defaults;
		$auto_form_defaults["default_layout"] = "table";
		
		$page = $this->page; // Pass through to code include
		
		$template = file_get_contents(Fakoli::resolveResource("/fakoli/admin.tpl"));
		
		//$template = ComponentManager::fireEvent("PreProcessPage", $template);
		
		ob_start();
		
		try
		{
			include $page->server_path;
		}
		catch(Exception $e)
		{
			echo $e->getMessage();
		}
		
		$content = ob_get_contents();
		ob_end_clean();
	
		//$content = ComponentManager::fireEvent("PostProcessContent", $content);
		
		$admin_menu = Fakoli::getAdminMenu();
		$icons = Fakoli::getAdminIcons();
		
		$menu = $this->formatMenu($admin_menu, $icons);
		
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
		
		$charset = strtolower(ini_get("default_charset"));
		$template = str_replace("{default_charset}", $charset, $template);
		
		preg_match_all("/\{config:(.*?)\}/", $template, $vars, PREG_SET_ORDER);
		
		foreach($vars as $var)
		{		
			$expr = $var[0];
			$name = $var[1];	
			$template = str_replace($expr, $config[$name], $template);
		}
		
		$component = $page->Component();
		
		$help = "<a href='#' onclick='popup(\"/action/admin/help?src={$component->name}&page={$page->identifier}\", \"help\", 940, 500, \"toolbar=0,location=0,scrollbars=1,resizable=1\"); return false;'> Help </a>";
		
	 	$template = str_replace("{help}", $help, $template);
	 	
		if ($config['prettyURLs'] === false)
		{
			$template = preg_replace("/(href|src|action)=(['\"])\\/?([\\w\\d_]+?)\\?/i", "$1=$2/page.php?identifier=$3&", $template);
			$template = preg_replace("/(href|src|action)=(['\"])\\/?([\\w\\d_]+?)[\"']/i", "$1=$2/page.php?identifier=$3$2", $template);
			$template = preg_replace("/go\\(([\"'])\\/?([\\w\\d_]+?)\\?/", "go($1/page.php?identifier=$2&", $template);
			$template = preg_replace("/go\\(([\"'])\\/?([\\w\\d_]+?)[\"']/", "go($1/page.php?identifier=$2$1", $template);
			
		}
				
		//$template = ComponentManager::fireEvent("PostProcessPage", $template);
		
		echo $template;		

		ComponentManager::fireEvent("RequestComplete");	
	}

	private function formatIcon($icon)
	{
		if (!$icon) return "<i class='fas fa-fw'></i>";
		if (endsWith($icon, ".svg"))
		{
			return "<i class='fas fa-fw' style='background: url({$icon}) center center no-repeat; background-size: contain; height:1em'></i>";
		}
		else if (preg_match('/^fa.?\s/', $icon))
		{
			return  "<i class='{$icon} fa-fw'></i>";
		}
		else
		{
			return "<i class='fas fa-{$icon} fa-fw'></i>";
		}
	}
	function formatMenu($admin_menu, $icons)
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
			$found = false;
			
			$icon = $this->formatIcon($icons[$section]);
			
			$submenu = " <h4 class=\"toggler atStart\">{$icon} {$section}</h4>\n <div class=\"atStart\">\n";

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
				
				$icon = $icons[$item];
				if (!$icon) $icon = $properties["icon"];
				
				$icon = $this->formatIcon($icon);
				
				if (checkRole($properties["role"]))
				{
					$submenu .= "<a class='$class' href='{$properties['page']}{$menu_params}'>{$icon} {$item}</a>\n";
					$found = true;
				}
			}
			
			$submenu .= "</div>";
			
			if ($found) 
			{
				++$menuSectionIdx;
				echo $submenu;
			}
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
			toggler.addClass("active");
		},
	 
		onBackground: function(toggler, element){
			toggler.removeClass("active");
		}
	}, document.id('accordion'));

	});
</script>
ENDSCRIPT;
		$menu = ob_get_contents();
		ob_end_clean();
		return $menu;
	}
}

/**@}*/

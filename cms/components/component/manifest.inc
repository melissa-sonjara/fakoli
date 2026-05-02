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

/** \defgroup component component
 * @{
 */

class ComponentManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "component";
		$component->description = "Component management for Fakoli/CMS";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 0;
		$component->enabled = true;
		
		return $component;
	}
	
	var $allow_sessionless_handlers = true;
	
	static function getAdminMenu()
	{
		return array
		(
			"Site Configuration"	=> array
			(

				"Components"	=>	array("page"		=>	"/admin/components",
									  "role"		=>	"admin",
								  	  "weight"	=>	0,
									  "icon"		=> 	"wrench"),
			
				"Import Configuration" => array("page"		=>	"/admin/import",
												"role"		=>	"admin",
												"weight"		=>	98,
												"icon"		=>	"upload"),
			
				"Export Configuration"	=>	array("page"		=>	"/admin/export",
												  "role"		=>	"admin",
												  "weight"	=>	99,
												  "icon"		=>	"download")
			)
		);		
	}	
	
	static function subscribeToEvents()
	{
		return array("ResolveIdentifier" 	=> 'ComponentManager::displayPage',
					 "EnumerateItems"		=> 'ComponentManager::enumerateComponentPages',
					 "DeleteRole"			=> 'ComponentManager::deleteRole',
					"upgradeComponent"		=>	'ComponentManager::upgradeComponent',
					"ComponentScanComplete" => 'ComponentManager::setDefaults'	
					);
					 
	}
	
	static function getStyles()
	{
		return array("/fakoli/css/tree.css", 
					 "/fakoli/css/tables.css",
					 "/fakoli/css/dialogs.css",
					 "/fakoli/css/textarea-resizer.css",
					 "/fakoli/css/context_menu.css",
					 "/fakoli/calendar/sonjara_calendar.css",
					 "/fakoli/css/multi_select.css",
				     "/fakoli/css/options_list.css",
					 "/fakoli/css/pushbutton.css",
					 "/fakoli/css/draggable_list.css",
					 "/fakoli/css/tinymce_branding.css",
					 "/fakoli/css/toggle.css"
		);
	}
	
	static function getScripts($ecmascript = false)
	{
		if ($ecmascript)
		{
			return array(
				"/fakoli/ecmascript/functions.js",
				"/fakoli/ecmascript/mootools-backfill.js",
				"/fakoli/tinymce/tinymce.min.js",
				"/fakoli/ecmascript/more/locale.js",
				"/fakoli/ecmascript/more/date.js",
				"/fakoli/ecmascript/more/string-extras.js",
				"/fakoli/ecmascript/more/array-extras.js",
				"/fakoli/ecmascript/more/number-format.js",
				"/fakoli/ecmascript/more/color.js",
				"/fakoli/ecmascript/more/class-extras.js",
				"/fakoli/ecmascript/more/form-extras.js",
				"/fakoli/ecmascript/more/hash.js",
				"/fakoli/ecmascript/more/uri.js",
				"/fakoli/ecmascript/more/assets.js",
				"/fakoli/ecmascript/more/group.js",
				"/fakoli/ecmascript/more/element-measure.js",
				"/fakoli/ecmascript/more/element-pin.js",
				"/fakoli/ecmascript/more/element-position.js",
				"/fakoli/ecmascript/more/mask.js",
				"/fakoli/ecmascript/more/spinner.js",
				"/fakoli/ecmascript/more/drag.js",
				"/fakoli/ecmascript/more/sortables.js",
				"/fakoli/ecmascript/more/slider.js",
				"/fakoli/ecmascript/more/scroller.js",
				"/fakoli/ecmascript/more/keyboard.js",
				"/fakoli/ecmascript/more/overtext.js",
				"/fakoli/ecmascript/more/fx-reveal.js",
				"/fakoli/ecmascript/more/fx-scroll.js",
				"/fakoli/ecmascript/more/fx-slide.js",
				"/fakoli/ecmascript/more/fx-accordion.js",
				"/fakoli/ecmascript/more/request-jsonp.js",
				"/fakoli/ecmascript/more/request-queue.js",
				"/fakoli/ecmascript/more/form-request.js",
				"/fakoli/ecmascript/more/form-validator.js",
				"/fakoli/ecmascript/dynamic_tab_bar.js",
				"/fakoli/ecmascript/ui.js",
				"/fakoli/ecmascript/iFrameFormRequest.js",
				"/fakoli/ecmascript/resizer.js",
				"/fakoli/ecmascript/calendar.js",
				"/fakoli/ecmascript/menu.js",
				"/fakoli/ecmascript/tree.js",
				"/fakoli/ecmascript/directory_tree.js",
				"/fakoli/ecmascript/context_menu.js",
				"/fakoli/ecmascript/paginating_table.js",
				"/fakoli/ecmascript/sorting_table.js",
				"/fakoli/ecmascript/filtering_table.js",
				"/fakoli/ecmascript/grouping_table.js",
				"/fakoli/ecmascript/scrolling_table.js",
				"/fakoli/ecmascript/draggable_table.js",
				"/fakoli/ecmascript/scrolling_tabs.js",
				"/fakoli/ecmascript/facet_manager.js",
				"/fakoli/ecmascript/subselect_manager.js",
				"/fakoli/ecmascript/multi_select.js",
				"/fakoli/ecmascript/auto_form_manager.js",
				"/fakoli/ecmascript/spreadsheet_form_manager.js",
				"/fakoli/ecmascript/expanding_list.js",
				"/fakoli/ecmascript/inline_editing.js",
				"/components/component/ecmascript/component_page_list.js"
			);
		}
		else
		{
			return array(
				"/fakoli/js/functions.js",
				"/fakoli/js/mootools-core-1.6.0.js",
				"/fakoli/js/mootools-more-1.6.0.js",
				"/fakoli/tinymce/tinymce.min.js",
				"/fakoli/js/ui.js",
				"/fakoli/js/iFrameFormRequest.js",
				"/fakoli/js/mootools-textarea-resizer.js",
				"/fakoli/calendar/sonjara_calendar.js",
				"/fakoli/js/menu.js",
				"/fakoli/js/tree.js",
				"/fakoli/js/directory_tree.js",
				"/fakoli/js/context_menu.js",
				"/fakoli/js/sorting_table.js",
				"/fakoli/js/paginating_table.js",
				"/fakoli/js/filtering_table.js",
				"/fakoli/js/grouping_table.js",
				"/fakoli/js/scrolling_table.js",
				"/fakoli/js/draggable_table.js",
				"/fakoli/js/scrolling_tabs.js",
				"/fakoli/js/facet_manager.js",
				"/fakoli/js/subselect_manager.js",
				"/fakoli/js/multi_select.js",
				"/fakoli/js/auto_form_manager.js",
				"/fakoli/js/spreadsheet_form_manager.js",
				"/fakoli/js/expanding_list.js",
				"/fakoli/js/inline_editing.js",
				"/fakoli/js/dynamic_tab_bar.js",
				"/components/component/js/component_page_list.js"
			);
		}
	}
}

/**@}*/
?>
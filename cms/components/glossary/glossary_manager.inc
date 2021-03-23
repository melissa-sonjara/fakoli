<?php
/**\addtogroup glossary
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the glossary component.
 */

class GlossaryManager
{
	function GlossaryManager()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("glossary", "glossarize_terms", false, "Boolean", "Adds definition tooltips to matching terms within page content", null, null, 1);
		Settings::setDefaultValue("glossary", "container_element", "body", "String", "DOM ID of the element to substitute within.", null, null, 2);
		Settings::setDefaultValue("glossary", "target_elements", "p,div,li", "String", "CSS Selector for the elements for which content can be glossarized", null, null, 3);
		Settings::setDefaultValue("glossary", "only_first_match", true, "Boolean", "Only attach the tooltip to the first matching term within the content area", null, null, 4);
		Settings::setDefaultValue("glossary", "tooltip_class", "glossary_tooltip", "String", "CSS class to use for the generated tooltips", null, null, 5);
		Settings::setDefaultValue("glossary", "term_class", "glossary_term", "String", "CSS class to use to wrap glossarized terms", null, null, 6);
		Settings::setDefaultValue("glossary", "exclude_class", "no_glossarize", "String", "CSS class used to indicate not to glossarize a page or element", null, null, 7);
	}

	static function glossarizeContent()
	{
		global $script;
		
		if (!Settings::getValue("glossary", "glossarize_terms")) return;
		
		$container = Settings::getValue("glossary", "container_element");
		$targets = Settings::getValue("glossary", "target_elements");
		$onlyFirst = Settings::getValue("glossary", "only_first_match") ? "true" : "false";
		$tooltipClass = Settings::getValue("glossary", "tooltip_class");
		$termClass = Settings::getValue("glossary", "term_class");
		$excludeClass = Settings::getValue("glossary", "exclude_class");
		
		$script .= <<<ENDSCRIPT
<script>
window.addEvent('domready', function()
{
	new Glossarizer('{$container}', '/action/glossary/glossary_list', 
	{
		selector: 		'{$targets}',
		firstOnly: 		{$onlyFirst},
		termClass:		'{$termClass}',
		tooltipClass: 	'{$tooltipClass}',
		excludeClass:	'{$excludeClass}'
	});
});
</script>
ENDSCRIPT;
	}

	static function upgradeComponent($version)
	{
		$mgr = new GlossaryUpgradeManager();
		$mgr->upgrade($version);
	}
}

/**@}*/?>
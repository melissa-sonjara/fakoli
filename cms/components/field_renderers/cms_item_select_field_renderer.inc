<?php
Fakoli::usingFeature("field_renderers");
Fakoli::using("component");

class CMSItemSelectFieldRenderer extends SelectFieldRenderer
{
	function CMSItemSelectFieldRenderer($form, $field, $label, $onChange = "")
	{
				
		$itemsByType = array();
		
		$itemsByType = ComponentManager::fireEvent("EnumerateItems", $itemsByType);
		
		$options = array();
		
		foreach($itemsByType as $group => $items)
		{
			$options[$group] = array();
			
			foreach($items as $item)
			{
				$options[$group][$item->identifier] = $item->format();
			}
		}
		
		$this->SelectFieldRenderer($form, $field, $label, $options, $onChange, true);		
	}
}

?>
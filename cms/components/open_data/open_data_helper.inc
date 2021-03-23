<?php
/**\addtogroup open_data
 * @{
 */



class OpenDataHelper
{
	function formatFieldLink($data)
	{
		if ($data->open_data_id)
		{
			$param = "open_data_id={$data->open_data_id}";
		}
		else
		{
			$param = "class={$data->class}&field={$data->field}";
		}
		$html = "<a href=\"open_data_form?$param\">{$data->field}</a>\n";
	
		return $html;
	}
	
	function compareClasses($itemA, $itemB)
	{
		if (strcasecmp($itemA->pretty_class_name, $itemB->pretty_class_name) == 0)
		{
			return 1;
		}
		elseif (strcasecmp($itemA->pretty_class_name, $itemB->pretty_class_name) < 0)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	
	function sortItems($items)
	{
		if(is_array($items))
		{
			usort($items, array(OpenDataHelper, compareClasses));
		}
		return $items;
	}
}
/**@}*/?>
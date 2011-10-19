<?php
Fakoli::using("section", "component");
Fakoli::usingFeature("field_renderers", "tree");

class CMSLinkSelectFieldRenderer extends FieldRenderer
{
	var $editableValue = true;
	var $size = 40;
	
	function CMSLinkSelectFieldRenderer(&$form, $field, $label)
	{
		$this->FieldRenderer(&$form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->overrides[$field]['label'] = $label;
		}
	}
	
	function createTree($field)
	{

		$itemsByType = array();
		$itemsByType = ComponentManager::fireEvent("EnumerateItems", &$itemsByType);
		$types = $this->categorizeItemTypes($itemsByType);
		
		$this->tree = new TreeControl("items");
		$this->tree->width = "320";
		$this->tree->height = "150";
		$this->tree->selectMode = "navigation";
		$this->tree->onSelect = "{$this->parent->id}_{$field}_selectItem";
		
		$sections = Query::create(Section, "ORDER BY section")->execute();
		
		foreach($sections as $section)
		{
			$sectionNode = new TreeNode($section->section, $section->section, null, false, "tree_node_closed", "tree_node_open");
			
			$content = $section->Content("ORDER BY identifier");
			$contentByIdentifier = reindexList($content, "identifier");
			
			foreach($itemsByType as $type => $items)
			{
				$category = new TreeNode("content_type_".codify($section->section)."_".codify($type), $type, null, false, "tree_node_closed", "tree_node_open");
			
				$categories[$type] = new TreeNode("category_".codify($section->section)."_".codify($type), $type, null, false, "tree_node_closed", "tree_node_open");
				
				foreach($items as $item)
				{
					if (!array_key_exists($item->identifier, $contentByIdentifier)) continue;
					
					$child = new TreeNode("available[{$item->identifier}]", $item->identifier, $this->getLink($section, $item->identifier), false, "plain_tree_closed", "plain_tree_open");
					$child->leafStyle = "file_node_leaf";
					
					$category->add($child);
				}
				
				$sectionNode->add($category);
			}
			
			$this->tree->add($sectionNode);
		}

	}
	
	function categorizeItemTypes($itemsByType)
	{
		$types = array();
		
		foreach($itemsByType as $type => $items)
		{
			foreach($items as $item)
			{
				$types[$item->identifier] = $type;
			}
		}
	
		return $types;
	}

	function getLink($section, $identifier)
	{
		if ($section->section == "/") 
			$url = "/$identifier";
		else
			$url = "/{$section->section}/$identifier";
		
		return $url;
	}
	
	function renderScript($field)
	{
		$script .= <<<ENDSCRIPT
<script type="text/javascript">
function {$this->parent->id}_{$field}_showTree(e)
{
	new Event(e).stop();
	var popup = $('{$this->parent->id}_{$field}_tree_popup');
	var txt = $('{$this->parent->id}_{$field}');
	var coords = txt.getCoordinates();
	popup.setStyles({'display': 'block'});
	var doc = $(document.body ? document.body : document.documentElement);
	doc.addEvent('click', {$this->parent->id}_{$field}_hideTree);
}	

function {$this->parent->id}_{$field}_hideTree()
{
	var popup = $('{$this->parent->id}_{$field}_tree_popup');
	popup.setStyles({'display': 'none'});
	var doc = $(document.body ? document.body : document.documentElement);
	doc.removeEvent('click', {$this->parent->id}_{$field}_hideTree);
}	
	
function {$this->parent->id}_{$field}_selectItem(url)
{
	var txt = $('{$this->parent->id}_{$field}');
	txt.set('value', url);
	{$this->parent->id}_{$field}_hideTree();
}

window.addEvent('domready', function()
{
	var popup = $('{$this->parent->id}_{$field}_tree_popup');
	popup.addEvent('click', function(e) { new Event(e).stop(); });
});
</script>
ENDSCRIPT;

		echo $script;
	}

	function renderSearchField($field, $mode = "")
	{
		$this->renderField($field);
	}
	
	function renderField($field = "")
	{
		if (!$field) $field = $this->field;
		
		$this->createTree($field);
		
		$this->_startField($field);

		$autocomplete = (!$this->autocomplete) ? "autocomplete='off' " : "";		
		$onchange = ($this->onChange) ? "onchange='$this->onChange(this)'" : "";		
		$disable = ($this->disable) ? "disabled='disabled'" : "";
		
		if ($this->editableValue)
		{
			echo "<input id='{$this->parent->id}_{$field}' type='text' name='$field' $onchange $autocomplete value='".htmlspecialchars($this->parent->data->get($field), ENT_QUOTES, 'UTF-8')."' size='{$this->size}' $disable/>";
			echo "&nbsp;<a class='button' href='#' onclick='{$this->parent->id}_{$field}_showTree(event); return false;'>Browse</a><br/><div id='{$this->parent->id}_{$field}_tree_popup' style='display: none;position:absolute;'>";
			$this->tree->writeHTML();
			echo "</div>";
		}
		else
		{
			echo "<input id='{$this->parent->id}_{$field}' type='hidden' name='$field' value='".htmlspecialchars($this->parent->data->get($field), ENT_QUOTES, 'UTF-8')."'/>";
			$this->tree->writeHTML();			
		}
		
		$this->_endField($field);
	}

	function renderReadOnly($field)
	{
		//TODO: Implement this
	}
}

?>
<?php
/**\addtogroup connectable
 * @{
 */


Fakoli::usingFeature("field_renderers");

class ConnectableTableFieldRenderer extends FieldRenderer
{
	var $table;
	
	var $field;
	var $label;
	
	var $targetClass;
	var $target;
			
	var $colspan = 2;
	var $related;

	function __construct($form, $field, $label, $targetClass)
	{
		parent::__construct($form);

		$this->field = $field;
		$this->label = $label;
		$this->targetClass = $targetClass;

		$this->target = new $targetClass;
		
		$this->parent->add($this);	
		
		$items = ConnectableManager::getConnectedItems($this->parent->data, $this->targetClass);
		$this->related = reindexList($items, $this->target->getPrimaryKey());
		$this->buildTable();
	}

	function buildTable()
	{
		$this->table = new DataListView(null, "connectables");
		$this->table->rowId = true;
		
		$this->table->pageSize = 5;
		$this->table->filter = true;
		$this->table->emptyMessage = "There are no ".$this->target->prettifyClassName(true)." to select from";

		$this->table->selectable = true;
		$this->table->selector(array($this, 'formatCheckbox'));
		
		$this->target->populateSelectionTable($this->table);
		
	}
	
	function formatCheckbox($item)
	{
		$pk = $item->getPrimaryKey();
		$pkval = $item->get($pk);
		
		$checked = array_key_exists($pkval, $this->related) ? " checked='checked'" : "";
		
		return $item->format("<input type='checkbox' class='checkbox' name='{$pk}[]' id='{$pk}_{{$pk}}' value='{{$pk}}'{$checked}/>");
	}
	
	function renderScript($field = "")
	{
		echo $this->table->writeScript();
	}
	
	function renderField($field)
	{
		echo $this->table->drawView();
	}
	
	function postProcess($field = "")
	{
		if (!$field) $field = $this->field;
	
		ConnectableManager::removeConnections($this->parent->data, $this->targetClass);
		
		$ids = $_POST[$this->target->getPrimaryKey()];
		
		foreach($ids as $id)
		{
			$target = new $this->target;
			$target->load($id);
			ConnectableManager::addConnection($this->parent->data, $target);
		}
		
		return true;
	}
}
/**@}*/?>
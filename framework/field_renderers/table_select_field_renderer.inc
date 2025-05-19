<?php
/**\addtogroup fieldrenderers
 * @{
 */

Fakoli::usingFeature("field_renderers");

class TableSelectFieldRenderer extends FieldRenderer
{
	var $table = null;

	var $field;
	var $label;

	var $colspan = 2;

	var $mode = "single"; ///< "single" or "multi" selection
	
	var $handler = null;
	var $processor = null;
	var $data = null;
	
	function __construct($form, $field, $label, $handler, $processor = null, $mode = "single", $data = null )
	{
		parent::__construct($form);

		$this->field = $field;
		$this->label = $label;
		$this->handler = $handler;
		$this->processor = $processor;
		$this->mode = $mode;
		$this->data = $data;
		
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			// JDG 8/14/2011 - allow "additional" fields to override label
			$form->overrides[$field]['label'] = $label;
		}
		
	}

	function buildTable($field)
	{
		$this->table = new DataListView($this->data, "{$field}_table");
		$this->table->rowId = true;

		$this->table->pageSize = 5;
		$this->table->filter = true;
		$this->table->emptyMessage = $this->emptyMessage;
		
		$this->table->selectable = true;
		$this->table->selector(array($this, 'formatSelector'));

		call_user_func_array($this->handler, array($this->table));
	}

	function formatSelector($item)
	{
		$pk = $item->getPrimaryKey();
		$pkval = $item->get($pk);

		switch($this->mode)
		{
		case "multi":
			$control = "checkbox";
			$name = "{$this->field}[]";
			break;
			
		case "single":
		default:
			$control = "radio";
			$name = $this->field;
			break;
		}
		
		$checked = array_key_exists($pkval, $this->related) ? " checked='checked'" : "";

		return $item->format("<input type='{$control}' class='{$control}' name='{$name}' id='{$this->field}_{{$pk}}' value='{{$pk}}'{$checked}/>");
	}

	function renderScript($field)
	{
		if (!$field) $field = $this->field;
		
		if (!$this->table)
		{
			$this->buildTable($field);
		}
		
		echo $this->table->writeScript();
	}

	function renderField($field)
	{
		if (!$field) $field = $this->field;
		
		if (!$this->table)
		{
			$this->buildTable($field);
		}
		
		$this->_startField($field);
		
		echo $this->table->drawView();
		
		$this->_endField($field);
	}

	function postProcess($field = "")
	{
		if (!$field) $field = $this->field;

		if ($this->processor)
		{
			call_user_func_array($this->processor, array($field));
		}

		return true;
	}
}

/**@}*/
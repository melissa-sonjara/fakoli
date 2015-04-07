<?php
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
	
	function __construct($form, $field, $label, $handler, $processor = null, $mode = "single" )
	{
		$this->FieldRenderer($form);

		$this->field = $field;
		$this->label = $label;
		$this->handler = $handler;
		$this->processor = $processor;
		$this->mode = $mode;
		
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
		$this->table = new DataListView(null, "{$field}_table");
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
		
		echo $this->table->drawView();
	}

	function postProcess($field = "")
	{
		if (!$field) $field = $this->field;

		if ($this->processor)
		{
			$this->processor($field);
		}

		return true;
	}
}
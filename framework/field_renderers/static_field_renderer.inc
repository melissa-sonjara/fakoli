<?php
/**\addtogroup fieldrenderers
 * @{
 */

class StaticFieldRenderer extends FieldRenderer
{
	function __construct($form, $field, $label)
	{
		$this->FieldRenderer($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			if ($label) $form->alias($field, $label);
		}
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		$obj = $this->parent->data;
		if ($obj->hasField($field))
		{
			echo $obj->get($field);
		}
		else
		{
			echo $obj->getDecoration($field);
		}
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		$this->renderField($field);
	}
}

/**@}*/
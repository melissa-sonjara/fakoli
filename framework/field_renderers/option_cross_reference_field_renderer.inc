<?php

class OptionCrossReferenceFieldRenderer extends FieldRenderer
{
	var $options = array();
	var $cssClass = "options_list";
	var $separator = ", ";
	var $nameField;
	var $xrefClass;
	var $onStartOption = null;
	
	function OptionCrossReferenceFieldRenderer(&$form, $field, $label, $options, $nameField, $xrefClass)
	{
		$this->options = $options;
		$this->nameField = $nameField;
		$this->xrefClass = $xrefClass;
		
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


	function groupBy($groupBy, $format = "")
	{
		$this->groupBy = $groupBy;
		$this->groupByFormat = $format;
	}
	
	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)) return;
		$id = "{$this->parent->id}_{$field}";
		?>
	<script type='text/javascript'>
	function <?echo $id?>_update(sel)
	{
		if (sel.hasClass("selected"))
		{
			sel.removeClass("selected");
		}
		else
		{
			sel.addClass("selected");
		}
		
		var values = [];
		
		$$('.<?echo $id?>_list > li').each(function(elt) 
		{ 
			if (elt.hasClass('selected'))
			{
				var v = elt.get('data-value');
				values.push(v);
			}
		});

		var hidden = document.id('<?echo $id?>');
		var v = hidden.value;
		hidden.value = values.join(",");
		var f = document.id('<?echo $this->parent->id?>');
		if (v != hidden.value && f && f.manager)
		{
			f.manager.dirty();
		}
	}
	</script>
<?
	}
	
	function renderField($field)
	{
		$this->_startField($field);
				
		if (count($this->options))
		{

			$obj = $this->parent->data;

			if (is_array($this->groupBy) && count($this->groupBy) > 0)
			{
				$keys = array_keys($this->options);
				$rc = $this->options[$keys[0]][0];
			}
			else
			{
				$rc = $this->options[0];
			}
			$pk = $obj->getPrimaryKey();
			
			if (!$rc)
			{
				$this->_endField($field);
				return;
			}
			
			$selectedItems = $this->getSelected($obj, $rc, $pk);

			
			$value = implode(",", array_keys($selectedItems));
			
			$id = "{$this->parent->id}_{$field}";
			
			echo "<input type='hidden' id='{$id}' name='{$field}' value='{$value}'/>";
					
			if (is_array($this->groupBy) && count($this->groupBy) > 0)
			{
				$gpk = $this->groupBy[0]->getPrimaryKey();
							
				foreach($this->groupBy as $group)
				{
					if (array_key_exists($group->get($gpk), $this->options))
					{
						echo $group->format($this->groupByFormat);
						$this->renderOptions($field, $this->options[$group->get($gpk)], $selected); 
					}
				}
			}
			else if (is_string($this->groupBy))
			{
				$options = regroupList($this->options, $this->groupBy);
	
				foreach($options as $group => $choices)
				{
					echo "<br/><h4>$group</h4>";
					$this->renderOptions($field, $choices, $selected);
				}
			}
			else
			{
				$this->renderOptions($field, $this->options, $selected);
			}
		}
		$this->_endField($field);
	}
	
	function renderOptions($field, $options, $selectedItems)
	{
		$id = "{$this->parent->id}_{$field}";

		echo "<ul class='{$id}_list {$this->cssClass}'>";
		foreach($options as $option)
		{
			$val = $option->get($option->getPrimaryKey());
			$classes = array();
			if (array_key_exists($val, $selectedItems)) $classes[] = "selected";
		
			if ($this->onStartOption)
			{
				$flag = call_user_func($this->onStartOption, $option);
				if ($flag === false) continue;
				if ($flag) $classes[] = $flag;
			}
		
			$classes = implode(" ", $classes);
		
			$description = $this->formatName($option, $this->nameField);
			echo "<li class='{$classes}' data-value='{$val}' onclick='{$id}_update(this); return false;'>{$description}</li>\n";
		}
		echo "</ul>";
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);
		
		if (count($this->options))
		{

			$obj = $this->parent->data;
			
			$rc = $this->options[0];
			$pk = $obj->getPrimaryKey();
				
			$selectedItems = $this->getSelected($obj, $rc, $pk);
			
			echo formatItems($selectedItems, $this->nameField, $this->separator);
		}
		
		$this->_endField($field);
	}
	
	function getSelected($obj, $rc, $pk)
	{
		return ($obj->get($pk) != "") ? indexedQuery($this->xrefClass, "WHERE {$pk}={$obj->get($pk)}", $rc->getPrimaryKey()) : array();
	}

	function getMatchingPK($xref, $obj)
	{
		$pks = $obj->getPrimaryKeyList();
		foreach($pks as $pk)
		{
			if ($xref->hasField($pk)) return $pk;
		}
	
		throw new DataItemException("No matching foreign key in xref table");
	}
	
	function postProcess($field = "")
	{
		if (count($this->options) == 0) return;

		$xref = new $this->xrefClass;
		
		$obj = $this->parent->data;
		$pk =  $this->getMatchingPK($xref, $obj);
		
		if (is_array($this->groupBy) && count($this->groupBy) > 0)
		{
			$keys = array_keys($this->options);
			$rc = $this->options[$keys[0]][0];
		}
		else
		{
			$rc = $this->options[0];
		}
		
		$optpk = $this->getMatchingPK($xref, $rc);
	
		$tx = new DataTransaction();
		try 
		{
			$this->clearOutRelations($obj, $pk, $tx);
		
			if (isset($_POST[$field]))
			{
				$ids = explode(",", $_POST[$field]);
					
				foreach($ids as $id)
				{
					$xref = new $this->xrefClass;
					$xref->joinTransaction($tx);
					$xref->set($pk, $obj->get($pk));
					$xref->set($optpk, $id);
					$xref->save();
				}
			}
			
			$tx->commit();
		} 
		catch (Exception $e) 
		{
			$tx->rollback();
			throw $e;
		}		
	}
	
	/**
	 * Clear out the current relations stored in the xref class.
	 *
	 * @param Object $obj
	 * @param String $pk
	 */
	function clearOutRelations($obj, $pk, $tx)
	{
		$xref = new $this->xrefClass;
		$xref->joinTransaction($tx);
		$xref->delete("WHERE {$pk}=".$obj->get($pk));
	}	
}

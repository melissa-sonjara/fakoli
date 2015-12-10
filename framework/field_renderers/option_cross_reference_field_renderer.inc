<?php

class OptionCrossReferenceFieldRenderer extends FieldRenderer
{
	var $options = array();
	var $cssClass = "options_list";
	var $separator = ", ";
	var $nameField;
	var $xrefClass;
	
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
		
		$$('#<?echo $id?>_list > li').each(function(elt) 
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

			$rc = $this->options[0];
			$pk = $obj->getPrimaryKey();
			
			$selectedItems = $this->getSelected($obj, $rc, $pk);

			
			$value = implode(",", array_keys($selectedItems));
			
			$id = "{$this->parent->id}_{$field}";
			
			echo "<input type='hidden' id='{$id}' name='{$field}' value='{$value}'/>";
			echo "<ul id='{$id}_list' class='{$this->cssClass}'>";
			foreach($this->options as $option)
			{
				$val = $option->get($option->getPrimaryKey());
				$selected = array_key_exists($val, $selectedItems) ? " class='selected'" : "";
				$description = $this->formatName($option, $this->nameField); 
				echo "<li{$selected} data-value='{$val}' onclick='{$id}_update(this); return false;'>{$description}</li>\n";	
			}
			echo "</ul>";
		}
		$this->_endField($field);
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
		$rc = $this->options[0];
	
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
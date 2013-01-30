<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/


/**
 * Field renderer for handling taxonomy term relationships.
 */
class TaxonomyTermFieldRenderer extends FieldRenderer
{
	var $field;
	var $label;
	var $taxonomy;
	var $dropdown;
	var $dropdownMaxWidth = "340px";
	var $dropdownMessage = "Click to Select Terms";
	var $options;
	var $showSelectAll;
	var $resizable = true;
	
	function TaxonomyTermFieldRenderer(&$form, $taxonomy)
	{
		$this->taxonomy = $taxonomy;
		$this->field = $taxonomy->identifier;
		$this->label = $taxonomy->taxonomy_name;
		$this->options = $taxonomy->Terms("ORDER BY term");
		$this->dropdown = true;
		
		$this->showSelectAll = true;
		
		$this->FieldRenderer($form);
		
		// JDG 7/31/2011 - allow override
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
	
	function setSize($width, $height)
	{
		$this->width = $width;
		$this->height = $height;
	}
	
	function groupBy($groupBy, $format = "")
	{
		$this->groupBy = $groupBy;
		$this->groupByFormat = $format;
	}
	
	function renderScript($field)
	{
		if ($this->showSelectAll)
		{
?>
<script type="text/javascript">

function <?echo $this->field ?>_toggleSelectAll(link)
{
	link = document.id(link);
	
	var value = (link.innerHTML == "Select All");

	var container = document.id('<?echo $this->field?>_container');
	
	container.getElements("input[type=checkbox]").each(function(cbox)
	{
		cbox.set('checked', value);
	});

	link.innerHTML = (value) ? "Deselect All" : "Select All";
	if (container.multiSelect) container.multiSelect.selectionChanged();
	return false;
}

</script>
<?
		}
		
		if ($this->dropdown)
		{
?>
<script type='text/javascript'>
window.addEvent('domready', function()
{
	new MultiSelect('<?echo $this->field?>_container', {maxWidth: '<?echo $this->dropdownMaxWidth?>', message: '<?echo jsSafe($this->dropdownMessage)?>'});
});
</script> 			
<?
		}
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
	
	function renderField($field = "")
	{
		if (count($this->options) == 0) return;
		
		$field = $this->field;
		
		$this->_startField($field);
		
		$obj = $this->parent->data;
		
		if ($this->showSelectAll)
		{
			echo "<a href='#' onclick='return {$field}_toggleSelectAll(this);'>Select All</a><br/>";
		}
		if ($this->width && $this->height)
		{
			$resizable = $this->resizable ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='{$field}_container' class='scrollbox{$resizable}{$dropdown}' style='width: {$this->width}px; height: {$this->height}px; overflow: auto'>\n";
		}
		
		$countOptions = count($this->options);
		
		$class = get_class($obj);
		$pk = $obj->get($obj->getPrimaryKey());
		
		$selected = IndexedQuery::create(TaxonomyTermAssociation, "WHERE taxonomy_id=:id AND class=:class AND id=:pk", "term_id")
								->bind(":id", $this->taxonomy->taxonomy_id, ":class", $class, ":pk", $pk)
								->execute();
								
		$this->renderCheckBoxes($field, $this->options, $selected);  

		if ($this->width && $this->height)
		{
			echo "</div>\n";
		}

		// Save this count to loop through checkboxes on required validation
		echo "<input type=\"hidden\" name=\"count_{$field}\" value=\"$countOptions\">";		
		
		$this->_endField($field);
	}
	
	function renderCheckboxes($field, $items, $selected)
	{
		foreach($items as $item)
		{
			$p = $item->term_id;
			
			echo "<input type='checkbox' class='checkbox' name='{$field}[$p]' id='checkbox_{$field}_$p' value='1'";
			if (array_key_exists($p, $selected))
			{
				echo " checked";
			}
			echo ">&nbsp;";
			echo "<label for='checkbox_{$field}_$p'>".$item->term."</label>";
			echo "<br/>";
		}
	}
	
	function renderReadOnly($field = "")
	{
		if (count($this->options) == 0) return;
//		
//		$xref = new $this->xrefClass;
//		
//		$obj = $this->parent->data;
//		$pk = $this->getMatchingPK($xref, $obj);
//		
//		if ($obj->get($pk) == "") return;
//		
//		$optpk = $this->getMatchingPK($xref, $this->options[0]);
//		
//		$items = indexedQuery($this->xrefClass, "WHERE {$pk}=".$obj->get($pk), $optpk);
//		
//		echo "<tr>\n";
//		$this->_printLabel($this->label);
//
//		echo "<td";
//		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
//		echo ">";
//
//		foreach($this->options as $item)
//		{
//			if (array_key_exists($item->$optpk, $items))
//			{
//				echo $this->formatName($item, $this->nameField)."<br/>";
//			}
//		}
//		
//		echo "</td></tr>\n";
	}
	
	function postProcess($field = "")
	{
		if (count($this->options) == 0) return;
		
		$obj = $this->parent->data;
		$class = get_class($obj);
		$pk = $obj->get($obj->getPrimaryKey());
		
		$assoc = new TaxonomyTermAssociation();
		$assoc->delete("WHERE class='$class' AND taxonomy_id={$this->taxonomy->taxonomy_id} AND id=$pk");
		
		if (isset($_POST[$this->field]))
		{
			foreach($_POST[$this->field] as $term_id => $dummy)
			{
				$assoc = new TaxonomyTermAssociation();
				$assoc->class = $class;
				$assoc->taxonomy_id = $this->taxonomy->taxonomy_id;
				$assoc->id = $pk;
				$assoc->term_id = $term_id;
				$assoc->save();
			}
		}
	}
}


?>
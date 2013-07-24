<?php
Fakoli::using("document");
Fakoli::usingFeature("field_renderers", "document_handler");

class DocumentSelectFieldRenderer extends FieldRenderer
{	
	var $library;
	var $emptyMessage = "No Document Selected";
	
	function DocumentSelectFieldRenderer(&$form, $field, $label, $libraryIdentifier)
	{
		$this->FieldRenderer($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->overrides[$field]['label'] = $label;
		}

		$this->library = DocumentLibrary::findByIdentifier($libraryIdentifier);
	}
	
	function writeDocumentList($field)
	{
		$documents = $this->library->Documents("ORDER BY title");
		
		foreach($documents as $document)
		{
			echo "<a href='#' onclick='{$this->parent->id}_{$field}_selectItem(this, {$document->document_id}); return false;'>";
			echo $this->formatDocument($document);
			echo "</a>";
		}
	}

	function formatDocument($document)
	{
		$img = DocumentHandler::getDocIcon($document->file);
		return "<img src='$img' alt='' style='display: inline-block; vertical-align: middle'>&nbsp;{$document->title}";
	}
		
	function formatDocumentLink($document)
	{
		$library = $document->DocumentLibrary();
		$size = $library->getFileSize($this->item);
		$img = DocumentHandler::getDocIcon($document->file);
	
		return "<img src='$img' alt='' style='display: inline-block; vertical-align: middle'>&nbsp;".
			   "<a href='/action/document/download?document_id={$value}'>{$document->title}</a>&nbsp;$size";
	}
	
	function renderScript($field)
	{
		if ($this->parent->isReadOnly($field)) return;
		
		$script .= <<<ENDSCRIPT
<script type="text/javascript">
function {$this->parent->id}_{$field}_showList(e)
{
	new Event(e).stop();
	var popup = $('{$this->parent->id}_{$field}_list_popup');
	var txt = $('{$this->parent->id}_{$field}');
	var coords = txt.getCoordinates();
	popup.setStyles({'display': 'block'});
	var doc = $(document.body ? document.body : document.documentElement);
	doc.addEvent('click', {$this->parent->id}_{$field}_hideList);
}
	
function {$this->parent->id}_{$field}_hideList()
{
	var popup = $('{$this->parent->id}_{$field}_list_popup');
	popup.setStyles({'display': 'none'});
	var doc = $(document.body ? document.body : document.documentElement);
	doc.removeEvent('click', {$this->parent->id}_{$field}_hideList);
}
	
function {$this->parent->id}_{$field}_selectItem(elt, id)
{
	var hidden = $('{$this->parent->id}_{$field}');
	
	var txt = $('{$this->parent->id}_{$field}_text');
	hidden.set('value', id);
	
	if (id)
	{
		txt.set('html', elt.get('html'));
	}
	
	{$this->parent->id}_{$field}_hideList();
}
	
window.addEvent('domready', function()
{
	var popup = $('{$this->parent->id}_{$field}_list_popup');
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
	
		$this->_startField($field);
	
		$autocomplete = (!$this->autocomplete) ? "autocomplete='off' " : "";
	
		$value = $this->parent->data->get($field);
		$text = "";
		if ($value)
		{
			$document = new Document($value);
			$text = $this->formatDocument($document);
		}
		else
		{
			$text = $this->emptyMessage;
		}
		
		echo "<input id='{$this->parent->id}_{$field}' type='hidden' name='$field' value='".htmlspecialchars($value, ENT_QUOTES, 'UTF-8')."'/>";
		echo "<span id='{$this->parent->id}_{$field}_text' class='document_select_text' onclick='{$this->parent->id}_{$field}_showList(event); return false;'>".$text."</span>";
		echo "&nbsp;<a class='button' href='#' onclick='{$this->parent->id}_{$field}_showList(event); return false;'>Browse</a><br/>";
		echo "<div id='{$this->parent->id}_{$field}_list_popup'  class='progressive_search' style='display: none;position:absolute; max-height: 150px; overflow-y: auto'>";
		$this->writeDocumentList($field);
		echo "</div>";

		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);
		
		$value = $this->parent->data->get($field);
		$text = "";
		if ($value)
		{
			$document = new Document($value);
			$text = $this->formatDocumentLink($document);
		}
		else
		{
			$text = $this->emptyMessage;
		}
		
		if ($this->parent->readOnlyForm)
		{
			echo $text;
		}
		else
		{
			echo "<input id='{$this->parent->id}_{$field}' type='hidden' name='$field' value='".htmlspecialchars($value, ENT_QUOTES, 'UTF-8')."'/>";
			echo "<span id='{$this->parent->id}_{$field}_text' class='document_select_text'>".$text."</span>";
		}
		
		$this->_endField($field);
	}
}
	
?>
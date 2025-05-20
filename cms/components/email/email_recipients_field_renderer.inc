<?php
/**\addtogroup email
 * @{
 */


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

Fakoli::usingFile("framework/field_renderers/text_field_renderer.inc");

/*
 * For forms that use a field "recipients" for entry of
 * multiple email addresses. If the user enters inconsistent
 * delimiters between email addresses, change to consistent ","
 * on preprocess.
 */
class EmailListFieldRenderer extends TextFieldRenderer
{
	var $removeDuplicates = false;
	
	function __construct(&$form, $field, $label = "")
	{
		parent::__construct($form);
		
		$label = ($label) ? $label : prettify($field);
		
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
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		$this->parent->validator->add(new EmailListValidator($field, $label));
		
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}
	}
	
	
	function renderField($field)
	{
		$recipients = $this->parent->data->get($field);
		$recipients = preg_replace("/,\s*/", ", ", $recipients);
		$this->parent->data->set($field, $recipients);
		
		parent::renderField($field);
	}
	
	function preProcess($field = "")
	{
		if(!$field) $field = "recipients";
		
		$recipients = EmailListFieldRenderer::cleanup($this->parent->data->get($field));
		
		if($this->removeDuplicates)
		{
			$recipients = implode(",", array_unique(explode(",", $recipients)));
		}
		
		$this->parent->data->set($field, $recipients);
	}

	/**
	 * Given a string of emails, clean up the list, removing any ";" or extra spaces,
	 * or trailing punctuation.
	 * 
	 * @param String $recipients
	 */
	static function cleanup($recipients)
	{
		$patterns = array("/,\s+/", "/;\s+/");
		$replacements = array(",", ",");
		$recipients = preg_replace($patterns, $replacements, $recipients);
		$recipients = rtrim($recipients, ",;");
		
		return $recipients;
	}
}


/**
 * Displays email recipients in mail client type of bubble inside a scrollbox.
 * 
 * @author janice
 *
 */
class EmailRecipientScrollBoxFieldRenderer extends FieldRenderer
{
	var $cssClass = 'email_recipient_scrollbox';
	var $field;
	var $nameField;
	var $recipients;
	var $delete_fn;				///< js function to delete selected recipient using "x"
	var $delete_all_fn;			///< js function to delete all recipients
	var $deleteLabel = "Clear All";
	
	function __construct(&$form, $field, $label = "",  $recipients, $nameField, $delete_fn = "", $delete_all_fn)
	{
		$this->recipients = $recipients;
		$this->field = $field;
		$this->nameField = $nameField;
		
		parent::__construct($form);
		
		$label = ($label) ? $label : prettify($field);
		
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->overrides[$field]['label'] = $label;
		}		
		
		$this->delete_fn = $delete_fn;	
		$this->delete_all_fn = $delete_all_fn;
		$this->colspan = 2;
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		$recipients = $this->recipients;
		
		echo "<div id='email_recipients'>\n";
			
		if($this->delete_all_fn)
		{
			$pk = $this->parent->data->getPrimaryKey();
			$id = $this->parent->data->$pk;
			echo "<a class='button' href='' onclick=\"{$this->delete_all_fn}($id); return false;\">{$this->deleteLabel}</a>\n";
		}
		echo "<div id='{$this->parent->id}_{$field}' class='{$this->cssClass}' name='$field'>\n";
		
		echo $this->formatRecipients($recipients);
		
		echo "</div>\n";
		echo "</div>\n";
		
		$this->_endField($field);
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
	
		$this->parent->validator->add(new EmailListValidator($field, $label));
	
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}
	}
		
	
	function renderReadOnly($field)
	{
		$this->delete_fn = "";
		$this->delete_all_fn = "";
		$this->renderField($field);
	}
		
	function formatRecipients($recipients)
	{
		if(!count($recipients)) return "";
		
		foreach($recipients as $recipient)
		{
			$html .= $this->formatOneRecipient($recipient);
		}		
		
		return $html;
	}
	
	
	/**
	 * Construct a list member using the id and value, such as
	 * id: "user_id_7"
	 * value "Sam Jones"
	 * 
	 * @param String $id
	 * @param String $value
	 */
	function formatOneRecipient($recipient)
	{
		$pk = $recipient->getPrimaryKey();
		$nameField = $this->nameField;
		if($this->delete_fn)
		{
			$removeLink = "<a class='remove_recipient' href='#' onclick='{$this->delete_fn}({{$pk}}); return false;'>x</a>";
		}
		
		return $recipient->format("<div id='{$pk}_{{$pk}}' class='email_recipient'>{$nameField}{$removeLink}</div>");
	}
}
/**@}*/?>
